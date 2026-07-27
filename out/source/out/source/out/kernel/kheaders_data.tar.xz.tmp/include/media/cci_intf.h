

#ifndef _CCI_INTF_H_
#define _CCI_INTF_H_

#include <linux/ioctl.h>
#include <linux/videodev2.h>

#define MSM_CCI_INTF_MAX_XFER 256

struct msm_cci_intf_register {
	unsigned short width; 
	unsigned short addr;
};

struct msm_cci_intf_data {
	unsigned short count; 
	unsigned short width;
	uint8_t buf[MSM_CCI_INTF_MAX_XFER];
};

struct msm_cci_intf_xfer {
	unsigned short cci_device;  
	unsigned short cci_bus;     
	unsigned short slave_addr;  
	struct msm_cci_intf_register  reg;
	struct msm_cci_intf_data      data;
};

#define MSM_CCI_INTF_READ \
	_IOWR('X', BASE_VIDIOC_PRIVATE + 50, struct msm_cci_intf_xfer)
#define MSM_CCI_INTF_READ32 \
	_IOWR('X', BASE_VIDIOC_PRIVATE + 50, struct msm_cci_intf_xfer)

#define MSM_CCI_INTF_WRITE \
	_IOWR('X', BASE_VIDIOC_PRIVATE + 51, struct msm_cci_intf_xfer)
#define MSM_CCI_INTF_WRITE32 \
	_IOWR('X', BASE_VIDIOC_PRIVATE + 51, struct msm_cci_intf_xfer)

#endif
