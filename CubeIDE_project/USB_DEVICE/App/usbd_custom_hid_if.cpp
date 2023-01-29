/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v1.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <usbd_custom_hid_if.hpp>
#include "data.hpp"
//#include "data.hpp"

 extern uint8_t OUT_Buffer[HID_OUT_BUFFER_SIZE];
 extern uint8_t FEATURE_Buffer[HID_FATURE_BUFFER_SEIZE];
/* USER CODE BEGIN INCLUDE */

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */
__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END =
{
	    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
	    0x09, 0x04,                    // USAGE (Joystick)
	    0xa1, 0x01,                    // COLLECTION (Application)
	    0xa1, 0x02,                    //   COLLECTION (Logical)
	    0x06, 0x00, 0xff,              //     USAGE_PAGE (Generic Desktop)
	    0x09, 0x00,                    //     USAGE (Undefined)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
	    0x75, 0x08,                    //     REPORT_SIZE (8)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
		0xb1, 0x22,                    //     feature (Data,Var,Abs,NPrf)
	    0x09, 0x01,                    //     USAGE (Vendor Usage 1)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x27, 0xff, 0xff, 0x00, 0x00,  //     LOGICAL_MAXIMUM (65535)
	    0x75, 0x10,                    //     REPORT_SIZE (16)
	    0x95, 0x02,                    //     REPORT_COUNT (2)
		0xb1, 0x22,                    //     feature (Data,Var,Abs,NPrf)
	    0xc0,                          //   END_COLLECTION
	    0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x09, 0xb8,                    //     USAGE (Elevator)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x09, 0xb0,                    //     USAGE (Aileron)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x09, 0xba,                    //     USAGE (Rudder)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x09, 0xbf,                    //     USAGE (Toe Brake)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x09, 0xbf,                    //     USAGE (Toe Brake)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x09, 0xbb,                    //     USAGE (Throttle)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x22,                    //     INPUT (Data,Var,Abs,NPrf)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x0b, 0x36, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Slider)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x22,                    //     INPUT (Data,Var,Abs,NPrf)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x0b, 0x36, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Slider)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x22,                    //     INPUT (Data,Var,Abs,NPrf)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x09, 0xc5,                    //     USAGE (Brake)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x22,                    //     INPUT (Data,Var,Abs,NPrf)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x05, 0x09,                    //     USAGE_PAGE (Button)
	    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
	    0x29, 0x20,                    //     USAGE_MAXIMUM (Button 32)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
	    0x95, 0x20,                    //     REPORT_COUNT (32)
	    0x75, 0x01,                    //     REPORT_SIZE (1)
	    0x81, 0x22,                    //     INPUT (Data,Var,Abs,NPrf)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x01,                    //   COLLECTION (Application)
	    0x05, 0x08,                    //     USAGE_PAGE (LEDs)
	    0x19, 0x01,                    //     USAGE_MINIMUM (Num Lock)
	    0x29, 0x02,                    //     USAGE_MAXIMUM (Caps Lock)
	    0x95, 0x08,                    //     REPORT_COUNT (8)
	    0x75, 0x01,                    //     REPORT_SIZE (1)
	    0x91, 0x22,                    //     OUTPUT (Data,Var,Abs,NPrf) //first OUT byte LEDs
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x0b, 0x36, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Slider)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x0b, 0x02, 0x00, 0x00, 0xff,  //     USAGE (Vendor Defined Page 1:Vendor Usage 2)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x00,              //     LOGICAL_MAXIMUM (255)
	    0x75, 0x08,                    //     REPORT_SIZE (8)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x91, 0x22,                    //     OUTPUT (Data,Var,Abs,NPrf) //second OUT byte FLAPS
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x0b, 0x36, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Slider)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x0b, 0xb3, 0x00, 0x02, 0x00,  //   USAGE (Simulation Controls:Auto-pilot enable)
	    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	    0x75, 0x01,                    //     REPORT_SIZE (1)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x91, 0x22,                    //   OUTPUT (Data,Var,Abs,NPrf)
	    0x75, 0x07,                    //   REPORT_SIZE (7)
	    0x91, 0x23,                    //     OUTPUT (Cnst,Var,Abs,NPrf)
	    0x0b, 0x02, 0x00, 0x00, 0xff,  //   USAGE (Vendor Defined Page 1:Vendor Usage 2)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x27, 0xff, 0xff, 0x00, 0x00,  //     LOGICAL_MAXIMUM (65535)
	    0x75, 0x10,                    //     REPORT_SIZE (16)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x91, 0x22,                    //     OUTPUT (Data,Var,Abs,NPrf)
	    0xc0,                          //   END_COLLECTION
	    0xa1, 0x00,                    //   COLLECTION (Physical)
	    0x0b, 0x36, 0x00, 0x01, 0x00,  //     USAGE (Generic Desktop:Slider)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x26, 0xff, 0x0f,              //     LOGICAL_MAXIMUM (4095)
	    0x75, 0x0c,                    //     REPORT_SIZE (12)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
	    0x75, 0x04,                    //     REPORT_SIZE (4)
	    0x81, 0x03,                    //     INPUT (Cnst,Var,Abs)
	    0x0b, 0xb3, 0x00, 0x02, 0x00,  //   USAGE (Simulation Controls:Auto-pilot enable)
	    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
	    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
	    0x75, 0x01,                    //     REPORT_SIZE (1)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x91, 0x22,                    //   OUTPUT (Data,Var,Abs,NPrf)
	    0x75, 0x07,                    //   REPORT_SIZE (7)
	    0x91, 0x23,                    //     OUTPUT (Cnst,Var,Abs,NPrf)
	    0x0b, 0x02, 0x00, 0x00, 0xff,  //   USAGE (Vendor Defined Page 1:Vendor Usage 2)
	    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
	    0x27, 0xff, 0xff, 0x00, 0x00,  //     LOGICAL_MAXIMUM (65535)
	    0x75, 0x10,                    //     REPORT_SIZE (16)
	    0x95, 0x01,                    //     REPORT_COUNT (1)
	    0x91, 0x22,                    //     OUTPUT (Data,Var,Abs,NPrf)
	    0xc0,                          //   END_COLLECTION
	    0xc0                           // END_COLLECTION
};
/** Usb custom HID report descriptor. */
//__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_HS[HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END = HID_MOUSE_ReportDesc;
/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceHS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_HS(void);
static int8_t CUSTOM_HID_DeInit_HS(void);
static int8_t CUSTOM_HID_OutEvent_HS(uint8_t * buffer);
static int8_t CUSTOM_HID_E0OutEvent_HS(uint8_t * buffer);
/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_HS =
{
  HID_MOUSE_ReportDesc,
  CUSTOM_HID_Init_HS,
  CUSTOM_HID_DeInit_HS,
  CUSTOM_HID_OutEvent_HS,
  CUSTOM_HID_E0OutEvent_HS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_HS(void)
{
  /* USER CODE BEGIN 8 */
  return (USBD_OK);
  /* USER CODE END 8 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_HS(void)
{
  /* USER CODE BEGIN 9 */
  return (USBD_OK);
  /* USER CODE END 9 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_HS(uint8_t * buffer)
{
/* copy data from OUT Endpoint to the OUT_Buffer*/
	memcpy(OUT_Buffer, buffer, HID_OUT_BUFFER_SIZE);


    /* Start next USB packet transfer once data processing is completed */
  USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceHS);

  return (USBD_OK);
  /* USER CODE END 10 */
}

/* USER CODE BEGIN 11 */

static int8_t CUSTOM_HID_E0OutEvent_HS(uint8_t * buffer)
{
/* copy data from OUT Endpoint to the OUT_Buffer*/
	//memcpy(OUT_Buffer, buffer, HID_OUT_BUFFER_SIZE);
	memcpy(FEATURE_Buffer, buffer, HID_FATURE_BUFFER_SEIZE);
    /* Start next USB packet transfer once data processing is completed */
  USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceHS);

  return (USBD_OK);
  /* USER CODE END 10 */
}
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_HS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceHS, report, len);
}
*/
/* USER CODE END 11 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

