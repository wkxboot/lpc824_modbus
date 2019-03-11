#include "board.h"
#include "cmsis_os.h"
#include "task_msg.h"
#include "scale_task.h"
#include "protocol_task.h"
#include "nxp_serial_uart_hal_driver.h"
#include "log.h"


int protocol_serial_handle;


osThreadId protocol_task_hdl;
osMessageQId protocol_task_msg_q_id;
static  uint8_t scale_addr;
typedef enum
{
    ADU_HEAD_STEP = 0,
    ADU_PDU_STEP,
    ADU_CRC_STEP
}adu_step_t;



/*通信协议部分*/
/*ADU*/
#define  ADU_SIZE_MAX                  20
#define  ADU_HEAD_OFFSET               0
#define  ADU_HEAD_SIZE                 2
#define  ADU_HEAD0_VALUE               'M'
#define  ADU_HEAD1_VALUE               'L'
#define  ADU_PDU_SIZE_REGION_OFFSET    2
#define  ADU_PDU_SIZE_REGION_SIZE      1
#define  ADU_PDU_OFFSET                3
#define  ADU_CRC_SIZE                  2
/*PDU*/
#define  PDU_SIZE_MIN                  2
#define  PDU_CODE_NET_WEIGHT           0
#define  PDU_CODE_REMOVE_TARE_WEIGHT   1
#define  PDU_CODE_CALIBRATION_ZERO     2
#define  PDU_CODE_CALIBRATION_FULL     3
#define  PDU_CODE_SENSOR_ID            4
#define  PDU_CODE_FIRMWARE_VERSION     5
#define  PDU_CODE_SET_ADDR             6
#define  PDU_CODE_MAX                  PDU_CODE_SET_ADDR

#define  PDU_CONTROLLER_ADDR           1
/*协议错误码*/
#define  PDU_NET_WEIGHT_ERR_VALUE      0x7FFF
#define  PDU_SUCCESS_VALUE             0x00
#define  PDU_FAILURE_VALUE             0x01

/*协议时间*/
#define  ADU_WAIT_TIMEOUT              osWaitForever
#define  ADU_FRAME_TIMEOUT             2
#define  ADU_RSP_TIMEOUT               200
#define  ADU_SEND_TIMEOUT              5



/* Table of CRC values for high-order byte */
static const uint8_t table_crc_hi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/* Table of CRC values for low-order byte */
static const uint8_t table_crc_lo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};
/*
* @brief 计算接收的数据CRC
* @param adu 接收缓存
* @param buffer_length 数据缓存长度
* @return CRC
* @note
*/

static uint16_t calculate_crc16(uint8_t *adu, uint16_t size)
{
    uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
    uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
    uint32_t i; /* will index into CRC lookup */

   /* calculate the CRC  */
    while (size --) {
        i = crc_hi ^ *adu++; 
        crc_hi = crc_lo ^ table_crc_hi[i];
        crc_lo = table_crc_lo[i];
    }

    return (crc_hi << 8 | crc_lo);
}
/*
* @brief 计算发送缓存CRC并填充到发送缓存
* @param adu 回应缓存
* @param size 当前数据缓存长度
* @return 加上crc后的数据长度
* @note
*/

static uint8_t adu_add_crc16(uint8_t *adu,uint8_t size)
{
    uint16_t crc16;
    crc16 = calculate_crc16(adu,size);

    adu[size ++] = crc16 & 0xff;
    adu[size ++] = crc16 >> 8;
    return size;
}
/*
* @brief 
* @param
* @param
* @return 
* @note
*/

static int request_net_weight(uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;
 
    scale_msg.type = TASK_MSG_REQ_NET_WEIGHT;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg =  *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_NET_WEIGHT){
            value[0] = msg.value & 0xFF;  
            value[1] = msg.value >> 8; 
            return 2;
        }      
    }

    return -1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

static int request_remove_tare_weight(uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;
 
    scale_msg.type = TASK_MSG_REQ_REMOVE_TARE_WEIGHT;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    value[0] = PDU_FAILURE_VALUE;
    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg = *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_REMOVE_TARE_WEIGHT && msg.value == SCALE_TASK_SUCCESS){
           value[0] = PDU_SUCCESS_VALUE;
        }    
    }

    return 1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

static int request_calibration_weight(int16_t weight,uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;
 
    if (weight == 0){
        scale_msg.type = TASK_MSG_REQ_CALIBRATE_ZERO;
    }else{
        scale_msg.type = TASK_MSG_REQ_CALIBRATE_FULL; 
    } 
    scale_msg.value = weight;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);
    value[0] = PDU_FAILURE_VALUE;

    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg = *(task_message_t *)&os_msg.value.v;
        if ((msg.type == TASK_MSG_RSP_CALIBRATE_ZERO || msg.type == TASK_MSG_RSP_CALIBRATE_FULL) && msg.value == SCALE_TASK_SUCCESS){
           value[0] = PDU_SUCCESS_VALUE;
        }
    }

    return 1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

static int request_sensor_id(uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;

    scale_msg.type = TASK_MSG_REQ_SENSOR_ID;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg =  *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_SENSOR_ID){
            value[0] = (uint8_t)msg.value;
            return 1;
        }  
    }

    return -1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/
static int request_firmware_version(uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;

    scale_msg.type = TASK_MSG_REQ_FW_VERSION;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg =  *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_FW_VERSION){
            value[0] = (uint16_t)msg.value & 0xFF;
            value[1] = (uint16_t)msg.value >> 8;
            return 2;
        }  
    }

    return -1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

static int request_scale_addr(uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;

    scale_msg.type = TASK_MSG_REQ_ADDR;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg =  *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_ADDR){
            value[0] = (uint8_t)msg.value;
            return 0;
        }  
    }

    return -1;
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

int request_set_scale_addr(uint8_t addr,uint8_t *value)
{
    osStatus status;
    osEvent  os_msg;
    task_message_t msg,scale_msg;

    scale_msg.type = TASK_MSG_REQ_SET_ADDR;
    scale_msg.value = addr;
    status = osMessagePut(scale_task_msg_q_id,*(uint32_t*)&scale_msg,0);
    log_assert(status == osOK);

    value[0] = PDU_FAILURE_VALUE;
    os_msg = osMessageGet(protocol_task_msg_q_id,ADU_RSP_TIMEOUT);
    if (os_msg.status == osEventMessage){
        msg =  *(task_message_t *)&os_msg.value.v;
        if (msg.type == TASK_MSG_RSP_SET_ADDR && msg.value == SCALE_TASK_SUCCESS){
            value[0] = PDU_SUCCESS_VALUE;
            scale_addr = addr;/*更新新地址*/
        }  
    }

    return 1;
}
/*
* @brief 串口接收主机ADU
* @param handle 串口句柄
* @param adu 数据缓存指针
* @param wait_timeout 等待超时时间
* @return -1 失败
* @return  0 成功
* @note
*/
static int receive_adu(int handle,uint8_t *adu,uint32_t wait_timeout)
{
    int rc;
    int read_size,read_size_total = 0;
    uint32_t timeout;
    uint16_t crc_calculated;
    uint16_t crc_received;
    adu_step_t step;

    timeout = wait_timeout;
    read_size = ADU_HEAD_SIZE + ADU_PDU_SIZE_REGION_SIZE;
    read_size_total = 0;
    step = ADU_HEAD_STEP;
  
    while(read_size != 0) {
        rc = serial_select(handle,timeout);
        if (rc == -1) {
            log_error("adu select error.read total:%d. read size:%d.\r\n",read_size_total,read_size);
            return -1;
        }
        if (rc == 0) {
            log_error("adu select timeout.read total:%d. read size:%d.timeout:%d.\r\n",read_size_total,read_size,timeout);
            return -1;
        }
  
        rc = serial_read(handle,(char *)adu + read_size_total,read_size);
        if (rc == -1) {
            log_error("adu read error.read total:%d. read size:%d.\r\n",read_size_total,read_size);
            return -1;
        }
   
        /*打印接收的数据*/
        for (int i = 0;i < rc;i++){
            log_array("<%2X>\r\n", adu[read_size_total + i]);
        }
   
        read_size_total += rc;
        read_size -= rc;
   
        if (read_size == 0) {
            switch(step){
            /*接收到了协议头和数据长度域*/
            case ADU_HEAD_STEP:
                if (adu[ADU_HEAD_OFFSET]     == ADU_HEAD0_VALUE && \
                    adu[ADU_HEAD_OFFSET + 1] == ADU_HEAD1_VALUE) {
                    step = ADU_PDU_STEP;      
                    read_size = adu[ADU_PDU_SIZE_REGION_OFFSET];
                    if (read_size == 0){
                        log_error("adu err in size value:%d.\r\n",adu[ADU_PDU_SIZE_REGION_OFFSET]);
                        return -1;
                    }
                    timeout = ADU_FRAME_TIMEOUT;
                } else {
                    log_error("adu err in head value0:%d value1:%d.\r\n",adu[ADU_HEAD_OFFSET],adu[ADU_HEAD_OFFSET + 1]);
                    return -1;
                } 
                                                                    
                break;
            /*接收完成了PDU的数据*/
            case ADU_PDU_STEP:
                step = ADU_CRC_STEP;      
                read_size = ADU_CRC_SIZE;
                timeout = ADU_FRAME_TIMEOUT;
            break;
            /*接收完成了全部的数据*/
            case ADU_CRC_STEP:
                crc_calculated = calculate_crc16(adu,read_size_total - ADU_CRC_SIZE);
                crc_received = adu[read_size_total - ADU_CRC_SIZE] | adu[read_size_total - ADU_CRC_SIZE + 1] << 8;
                if (crc_calculated != crc_received) {
                    log_error("adu err in crc.recv:%d calculate:%d.\r\n",crc_received,crc_calculated);
                    return -1;
                } else {
                    return read_size_total;
                }
            break;
            default:
                log_error("adu internal err.\r\n");
                return -1;
                }
        }
    }
    log_error("adu internal err.\r\n");
    return -1;
}

/*
* @brief 解析pdu
* @param pdu 数据缓存指针
* @param size 数据大小
* @param adu 回应的数据缓存指针
* @return -1 失败 
* @return  > 0 回应的adu大小
* @note
*/

static int parse_pdu(uint8_t *pdu,uint8_t size,uint8_t addr,uint8_t *adu)
{
    int rc;
    uint8_t code;
    uint8_t scale_addr;
    uint8_t pdu_offset = 0;
    uint8_t adu_size = 0;
    uint16_t calibration_weight;
    uint8_t set_addr;

    if (size < PDU_SIZE_MIN) {
        log_error("pdu size:%d < %d err.\r\n",size,PDU_SIZE_MIN);
        return -1;
    }
    
    scale_addr = pdu[pdu_offset ++];
    if (scale_addr != addr) {
        log_error("pdu scale addr:%d != %d err.\r\n",scale_addr,addr);
        return -1;
    }
    code = pdu[pdu_offset ++];
    if (code > PDU_CODE_MAX) {
        log_error("pdu code:%d > %d err.\r\n",code,PDU_CODE_MAX);
        return -1;
    }
    /*构建回应ADU*/
    adu[adu_size ++] = ADU_HEAD0_VALUE;
    adu[adu_size ++] = ADU_HEAD1_VALUE;
    adu[adu_size ++] = 0;
    adu[adu_size ++] =  scale_addr;
    adu[adu_size ++] = code;

    switch (code) {
    case PDU_CODE_NET_WEIGHT:
        if (pdu_offset != size ) {
            log_error("pdu size:%d of net weight err.\r\n",size);
            return -1;
        }
        rc = request_net_weight(&adu[adu_size]);
        if (rc < 0) {
            log_error("req net weight err.\r\n");
            return -1;
        } 
        adu_size += rc;
        break;
     case PDU_CODE_REMOVE_TARE_WEIGHT:
        if (pdu_offset != size ) {
            log_error("pdu size:%d of remove tare weight err.\r\n",size);
            return -1;
        }
        rc = request_remove_tare_weight(&adu[adu_size]);
        if (rc < 0) {
            log_error("req remove tare weight fail.\r\n");
            return -1;
        }
        adu_size += rc;
        break;   
     case PDU_CODE_CALIBRATION_ZERO:
        calibration_weight = pdu[pdu_offset ++];
        calibration_weight|= pdu[pdu_offset ++] << 8;
        if (pdu_offset != size) {
            log_error("pdu size:%d of calibration zero err.\r\n",size);
            return -1;
        }

        rc = request_calibration_weight(calibration_weight,&adu[adu_size]);
        if (rc < 0) {
            log_error("req calibration zero fail.\r\n");
            return -1;
        }
        adu_size += rc;
        break;  
     case PDU_CODE_CALIBRATION_FULL:
        calibration_weight = pdu[pdu_offset ++];
        calibration_weight|= pdu[pdu_offset ++] << 8;
        if (pdu_offset != size) {
            log_error("pdu size:%d of calibration full err.\r\n",size);
            return -1;
        }

        rc = request_calibration_weight(calibration_weight,&adu[adu_size]);
        if (rc < 0) {
            log_error("req calibration full fail.\r\n");
            return -1;
        }
        adu_size += rc;
        break; 
     case PDU_CODE_SENSOR_ID:
        if (pdu_offset != size) {
            log_error("pdu size:%d of sensor id err.\r\n",size);
            return -1;
        }
        rc = request_sensor_id(&adu[adu_size]);
        if (rc < 0) {
            log_error("req sensor id fail.\r\n");
            return -1;
        }
        adu_size += rc;
        break; 
     case PDU_CODE_FIRMWARE_VERSION:
        if (size != pdu_offset) {
            log_error("pdu size:%d of firmware err.\r\n",size);
            return -1;
        } 
        rc = request_firmware_version(&adu[adu_size]);
        if (rc < 0) {
            log_error("request firmware err.\r\n");
            return -1;
        }
        adu_size += rc;
        break; 
     case PDU_CODE_SET_ADDR:
        set_addr = pdu[pdu_offset++];
        if (size != pdu_offset) {
            log_error("pdu size:%d of set addr err.\r\n",size);
            return -1;
        } 
        rc = request_set_scale_addr(set_addr,&adu[adu_size]);
        if (rc < 0) {
            log_error("request set scale addr err.\r\n");
            return -1;
        }
        adu_size += rc;
        break; 
    default:
        log_error("adu internal err.code:%d.\r\n",code);
        return -1;
    }

    /*添加CRC16*/
    adu[ADU_PDU_SIZE_REGION_OFFSET] = adu_size - ADU_HEAD_SIZE - ADU_PDU_SIZE_REGION_SIZE;
    adu_size = adu_add_crc16(adu,adu_size);
    return adu_size;
}


/*
* @brief 通过串口回应处理结果
* @param handle 串口句柄
* @param adu 结果缓存指针
* @param handle 串口句柄
* @param adu 结果缓存指针
* @param size 结果大小
* @param timeout 发送超时时间
* @return -1 失败 
* @return  0 成功 
* @note
*/
static int send_adu(int handle,uint8_t *adu,uint8_t size,uint32_t timeout)
{
    uint8_t write_size;

    write_size = serial_write(handle,(char *)adu,size);
    for (int i = 0; i < write_size; i++){
        log_array("[%2X]\r\n",adu[i]);
    }
    if (size != write_size){
        log_error("controller err in  serial write. expect:%d write:%d.\r\n",size,write_size); 
        return -1;      
     }
  
    return serial_complete(handle,timeout);
}

/*串口中断处理*/
void USART1_IRQHandler()
{
  nxp_serial_uart_hal_isr(protocol_serial_handle);
}

/*
* @brief 
* @param
* @param
* @return 
* @note
*/

void protocol_task(void const * argument)
{
    int rc; 


    uint8_t adu_recv[ADU_SIZE_MAX];
    uint8_t adu_send[ADU_SIZE_MAX];
 
    rc = serial_create(&protocol_serial_handle,PROTOCOL_TASK_RX_BUFFER_SIZE,PROTOCOL_TASK_TX_BUFFER_SIZE);
    log_assert(rc == 0);
    rc = serial_register_hal_driver(protocol_serial_handle,&nxp_serial_uart_hal_driver);
    log_assert(rc == 0);
 
    rc = serial_open(protocol_serial_handle,
                    PROTOCOL_TASK_SERIAL_PORT,
                    PROTOCOL_TASK_SERIAL_BAUDRATES,
                    PROTOCOL_TASK_SERIAL_DATABITS,
                    PROTOCOL_TASK_SERIAL_STOPBITS);
    log_assert(rc == 0); 
    log_debug("protocol task ok.\r\n");

    rc = request_scale_addr(&scale_addr);
    log_assert(rc == 0);
    /*清空接收缓存*/
    serial_flush(protocol_serial_handle);
    while (1) {

        /*接收主机发送的adu*/
        rc = receive_adu(protocol_serial_handle,(uint8_t *)adu_recv,ADU_WAIT_TIMEOUT);
        if (rc < 0) {
            /*清空接收缓存*/
            serial_flush(protocol_serial_handle);
            continue;
        }
        /*解析处理pdu*/
        rc = parse_pdu((uint8_t *)&adu_recv[ADU_PDU_OFFSET],rc - ADU_HEAD_SIZE - ADU_PDU_SIZE_REGION_SIZE - ADU_CRC_SIZE,scale_addr,adu_send);
        if (rc < 0) {
            continue;
        }
        /*回应主机处理结果*/
        rc = send_adu(protocol_serial_handle,adu_send,rc,ADU_SEND_TIMEOUT);
        if (rc < 0) {
            continue;
        }

    }    
}
