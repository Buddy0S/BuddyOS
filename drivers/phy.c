#include <stdint.h>
#include "memory.h"
#include "uart.h"
#include "net.h"

/*
 * phy.c
 *
 * This file contains the phy driver code to use 
 * Network capabilities of the BeagleBone Black
 * 
 * - Microchip LAN8710A PHY Driver
 * 
 * Author: Marwan Mostafa 
 *
 * */

/* --------------------------PHY CODE----------------------------- */

/*
 * phy_reset()
 *  - secret function
 *  - enables power/resets the PHY
 *  - this took forever to figure out
 *  - GPIO1_8 pin can be used to reset PHY
 *  - this was found in am335x-bone-common.dtsi from the linux kernel
 *  - needs to wait a max amount of time after aserting and deasserting PHY reset
 *  - buddy function can handle this
 *
 * */
void phy_reset(void)
{
    // Configure GPIO1_8 as an output by clearing its bit in the OE register.
    REG(GPIO_OE) &= ~PHY_RESET_BIT;

    // Assert PHY reset 
    REG(GPIO_DATAOUT) &= ~PHY_RESET_BIT;
    buddy();  

    // Deassert PHY reset
    REG(GPIO_DATAOUT) |= PHY_RESET_BIT;
    buddy();  
}

/*
 * phy_readreg()
 *  - reads a phy reg and returns its value
 *
 * */
uint16_t phy_readreg(uint8_t phy_addr, uint8_t reg_addr){
	
    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    REG(MDIOUSERACCESS0) = (PHY_READ | GO_BIT | (reg_addr << REG_ADDR_SHIFT) | (phy_addr << PHY_ADDR_SHIFT));

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    if ( REG(MDIOUSERACCESS0) & READ_ACK) {
        return REG(MDIOUSERACCESS0) & PHY_DATA;
    }

    return 2;
}

/*
 * phy_writereg()
 *  - writes data to a phy reg
 *
 * */
void phy_writereg(uint8_t phy_addr, uint8_t reg_addr, uint16_t data){

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}

    REG(MDIOUSERACCESS0) = (PHY_WRITE | GO_BIT | (reg_addr << REG_ADDR_SHIFT) | (phy_addr << PHY_ADDR_SHIFT) | data);

    while ( REG(MDIOUSERACCESS0) & GO_BIT){}
}

/*
 *  phy_get_autoneg_status()
 *   - returns status of auto negotiation
 *
 * */
int phy_get_autoneg_status(uint8_t phy_addr){

    return phy_readreg(phy_addr,PHY_BSR) & AUTO_NEG_COMPLETE;
}

/*
 * phy_autonegotiate()
 *  - sets transfer capabilites of port 1
 *  - advertizes our capabilites 
 *  - sets transfer mode based on capablities of partner
 *
 * */
int phy_autonegotiate(uint8_t phy_addr){

    uint16_t data;
    uint16_t data_a;
    uint16_t data_p;    
    uint32_t transfer;
    uint16_t advert = PHY100 | PHY100_FD | PHY10 | PHY10_FD;

    data = phy_readreg(phy_addr, PHY_BCR);

    data |= AUTO_NEG_ENABLE;

    phy_writereg(phy_addr, PHY_BCR, data);

    data = phy_readreg(phy_addr, PHY_BCR);

    data_a = phy_readreg(phy_addr, PHY_AUTONEG_ADV);

    data_a &= ~AUTO_NEG_CLEAR;

    data_a |= advert;

    phy_writereg(phy_addr, PHY_AUTONEG_ADV, data_a);

    data |= AUTO_NEG_RESTART;

    phy_writereg(phy_addr, PHY_BCR, data);

    // this loop is here if iwant to try multiple times but just gonna break at 1 attempt
    for (int i = 0; i < 1; i++){
    
	    uart0_printf("PHY attempting auto negotiation, this will take a while..\n"); 
      buddy();
	    if ( phy_get_autoneg_status(phy_addr) ) break;

	    if (i == 0){
	
	      uart0_printf("Auto Negotiation Failed\n");
		
	      return -1;
	    }
    }

    uart0_printf("Auto Negotiation Successful\n");

    data_p = phy_readreg(phy_addr, PHY_PARTNER_CAP);

    if ( data_p & PHY100_FD ){
        uart0_printf("Setting transfer Mode to 100mbps Full Duplex\n");
	transfer = FULL_DUPLEX;

    }else if ( data_p & PHY100 ){
        uart0_printf("Setting transfer Mode to 100mbps \n");	    
        transfer = HALF_DUPLEX;

    }else if ( data_p & PHY10_FD ){
        uart0_printf("Setting transfer Mode to 10mbps Full Duplex\n");
        transfer = IN_BAND | FULL_DUPLEX;

    }else if ( data_p & PHY10 ){
        uart0_printf("Setting transfer Mode to 10mbps\n");
	transfer = IN_BAND | HALF_DUPLEX;

    }else {
    
        uart0_printf("No valid Transfer\n");
	return -1;
    }

    cpsw_set_transfer(transfer);

}

/*
 * phy_alive()
 *  - returns if Ethernet PHY is Alive
 *
 * */
int phy_alive(){

    return REG(MDIOALIVE);
}

/*
 * phy_link()
 *  - returns if Ethernet PHY link is up
 *
 * */
int phy_link(){

    return REG(MDIOLINK);
}

/*
 * phy_init()
 *  - initializes the Ethernet PHY 
 *
 * */
int phy_init(){

    uart0_printf("Checking if PHY is Alive\n");

    if (phy_alive()){
        uart0_printf("Ethernet PHY Alive\n");
    }else {
        uart0_printf("Ethernet PHY Not Alive\n");
	    return -1;
    }

    phy_autonegotiate(PHY1);

    if (phy_link()){
        uart0_printf("Ethernet Cable Linked\n");
    }else {
        uart0_printf("Ethernet Cable Not Linked\n");
        return -1;
    }

    cpsw_enable_gmii();
    //uart0_printf("Enabling Frame Sending and Recieving\n"); 

    return 0;
}
