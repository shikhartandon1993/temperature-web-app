#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/i2c.h>

#include <zephyr/logging/log.h>

//#include <zephyr/device.h>
//#include <zephyr/devicetree.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/net/http/service.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/client.h>

#include <app/headers.h>

#define TEMP_SENSOR_I2C_ADDRESS 0x4F
#define EEPROM_MAC_I2C_ADDRESS  0x5E
#define TEMP_REG 0x00
#define MAC_REG  0x9A

LOG_MODULE_REGISTER(main);

#define PRIORITY 5
#define STACKSIZE 2048
#define THREAD_PRIORITY 7


//include the static HTML page which was converted from .html to C array using below command-->
//xxd -i static/index.html > src/index_html.inc
/*#include "index_html.inc"*/

//#include "index_html.h"


//extern const int my_http_resources_anchor;

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(io1_led),gpios);
const struct device *i2c_dev = DEVICE_DT_GET(DT_ALIAS(i2c_0));


HTTP_RESOURCE_DEFINE(index, my_http_service, "/", &index_resource_detail);
HTTP_RESOURCE_DEFINE(temp,  my_http_service, "/temp", &temp_resource_detail);

static uint16_t port = 8080;
HTTP_SERVICE_DEFINE(my_http_service,NULL,&port,1,4,NULL,NULL);/*Use HTTP_SERVICE_DEFINE() instead of HTTP_SERVICE_DEFINE_EMPTY() 
so that the service has a workqueue and can process requests, and ensure the resource list is linked.*/


int main(void)
{

    struct net_if *iface = net_if_get_default(); //get default ethernet interface
        
    char buf[NET_IPV4_ADDR_LEN];

    //print IP address of the board
    if (iface) {
        struct net_if_addr *if_addr = (struct net_if_addr*)(&iface->config.ip.ipv4->unicast[0]);
        printk("Assigned IP: %s",
                net_addr_ntop(AF_INET, &if_addr->address.in_addr, buf, sizeof(buf)));
    }
    else{
        while(1){printk("Fail!\n");}
    }

    //setting static MAC address of SAME54
    uint8_t mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01 };
    net_if_set_link_addr(iface, mac, sizeof(mac), NET_LINK_ETHERNET);

    // bring ethernet interface up
    net_if_up(iface);        
    printk("Ethernet interface is up\n");

    struct net_if_addr *if_addr_1 = (struct net_if_addr*)(&iface->config.ip.ipv4->unicast[0]);
    printk("[UP]Assigned IP: %s",
                net_addr_ntop(AF_INET, &if_addr_1->address.in_addr, buf, sizeof(buf)));
    
    /*struct net_linkaddr *linkaddr = net_if_get_link_addr(iface);

    printk("[UP]MAC from iface: %02X:%02X:%02X:%02X:%02X:%02X\n",
       linkaddr->addr[0], linkaddr->addr[1], linkaddr->addr[2],
       linkaddr->addr[3], linkaddr->addr[4], linkaddr->addr[5]);*/
  
    printk("I2C dev: %p\n", i2c_dev);
    printk("dev name: %s\n", i2c_dev->name);

    if(!gpio_is_ready_dt(&led))
    {
        printk("IO1 board LED not ready\n");
        return -1;
    }

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    gpio_pin_set_dt(&led, 1);   

    if(!(device_is_ready(i2c_dev)))
    {
        printk("I2C module not ready!");
        return -1;
    }

    printk("Scanning I2C bus!\n");

    for(uint8_t addr = 0x3;addr <= 0x77;addr++)
    {
        struct i2c_msg msgs[1];
        uint8_t dummy;

        msgs[0].buf = &dummy;
        msgs[0].len = 1;
        msgs[0].flags = I2C_MSG_READ | I2C_MSG_STOP;

        if(i2c_transfer(i2c_dev, &msgs[0], 1, addr) == 0)
        {
            printk("Found device at 0x%02X\n",addr);
        }
    }

    //dummy write
    /*{
        struct i2c_msg msgs[1];
        uint8_t dummy = 0x9A;

        msgs[0].buf = &dummy;
        msgs[0].len = 1;
        msgs[0].flags = I2C_MSG_WRITE | I2C_MSG_RESTART;

        if(i2c_transfer(i2c_dev, &msgs[0], 1, 0x5E) == 0)
        {
            printk("Dummy write complete!");
        }
        else
        {
            printk("Dummy write failed!");
        }
    }

    //MAC address read operation
    {
        uint8_t raw_mac[6];
        if(i2c_burst_read(i2c_dev,(uint16_t)(0x5E),(uint8_t)(0x9A),raw_mac,6) == 0)
        {
            printk("MAC address:%02X:%02X:%02X:%02X:%02X:%02X\n",raw_mac[0],raw_mac[1],
            raw_mac[2],raw_mac[3],raw_mac[4],raw_mac[5]);
        }
        else
        {
            printk("MAC address read failed!");
        }
    }*/

    /*uint8_t dummy[1] = {0x9A};
    uint8_t raw_mac[6];
    if(i2c_write_read(i2c_dev, 94,dummy, 1,raw_mac, 6) == 0)
    {
        printk("MAC address:%02X:%02X:%02X:%02X:%02X:%02X\n",raw_mac[0],raw_mac[1],
            raw_mac[2],raw_mac[3],raw_mac[4],raw_mac[5]);
    }
    else
    {
        printk("MAC address read failed!\n");
    }*/

    int ret = http_server_start();
    if (ret < 0) {
        LOG_ERR("HTTP server failed to start: %d", ret);
    } else {
        LOG_INF("HTTP server started on port %d", port);
        //http_service_enable(&my_http_service);
    }



    while(1)
    {
        uint8_t raw_temp[2];
        int ret;

        ret = i2c_burst_read(i2c_dev,TEMP_SENSOR_I2C_ADDRESS,TEMP_REG,raw_temp,2);
        if(ret != 0)
        {
            printk("I2C temperature read failed! %d\n",ret);
            k_sleep(K_MSEC(200));
            continue;
        }

        printk("I2C read succeeded \nraw_temp[0], = %u\nraw_temp[1] = %u\n",raw_temp[0],raw_temp[1]);
        
        uint16_t temp_val = (((uint16_t)raw_temp[0] << 8) | ((uint16_t)raw_temp[1]));
        int16_t temp_code = temp_val >> 7;
        
        //if temp is negative
        if((temp_code & (1 << 8)))
        {
            //temp_code &= ~(1 << 8);//unset 8th bit
            //temp_code = ~(temp_code) + 1;//2s complement of the positive value
            temp_code |= ~((1 << 9) - 1);
        }

        temp_c = temp_code * 0.5f;//9-bit resolution 0.5C change

        printk("Temp: %d C\n", temp_c);
        

        k_sleep(K_SECONDS(1));
    }
}


//K_THREAD_STACK_DEFINE(my_http_stack, 4096);
//static struct k_work_q my_http_workq;

/*k_work_queue_start(&my_http_workq, my_http_stack,
                       K_THREAD_STACK_SIZEOF(my_http_stack),
                       PRIORITY, NULL);*/

//extern const struct http_resource_desc _http_resource_desc_my_http_service_list_start[];
//index_html_len = (size_t)(_binary_app_static_index_html_end - _binary_app_static_index_html_start);
//index_resource_detail.static_data = _binary_app_static_index_html_start;
//index_resource_detail.static_data_len = index_html_len;

// Declare the linker symbols (defined in your sections-html.ld)
/*extern const unsigned char _binary_app_static_index_html_start[];
extern const unsigned char _binary_app_static_index_html_end[];*/

/*#define index_html      _binary_app_static_index_html_start
#define index_html_len   (_binary_app_static_index_html_end - _binary_app_static_index_html_start)*/


//static size_t index_html_len;

/*HTTP_SERVICE_DEFINE_EMPTY(my_http_service,NULL, &port, 1, 4, NULL, NULL);*/
/*HTTP_SERVICE_DEFINE(my_http_service, NULL, &port, 1, 4,
                    &my_http_workq, &my_http_stack);*/

//Assign static IP address and netmask
//net_if_ipv4_addr_add(iface, &ipaddr, NET_ADDR_MANUAL, 0);
//net_if_ipv4_set_netmask_by_addr(iface, &ipaddr, &netmask);

//static struct in_addr ipaddr = { { { 192, 168, 1, 100 } } };//IP address = 192.168.1.100
//static struct in_addr netmask = { { { 255, 255, 255, 0 } } };//netmask = 255.255.255.0

//(void)my_http_resources_anchor;

//configure_iface(iface);  // assign static IP
/*void configure_iface(struct net_if *iface)
{
    // (B) Assign static IP
    static struct in_addr ipaddr = { { { 192, 168, 1, 100 } } };
    static struct in_addr netmask = { { { 255, 255, 255, 0 } } };

    net_if_ipv4_addr_add(iface, &ipaddr, NET_ADDR_MANUAL, 0);
    net_if_ipv4_set_netmask_by_addr(iface, &ipaddr, &netmask);
}*/