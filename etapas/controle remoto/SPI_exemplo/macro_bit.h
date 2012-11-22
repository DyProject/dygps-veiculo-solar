#ifndef MACRO_BIT_H_INCLUDED
#define MACRO_BIT_H_INCLUDED

//--------------------------------------------------------------

#define set_bit(adress,bit) (adress |= (1<<bit))   
#define clr_bit(adress,bit) (adress &= ~(1<<bit)) 
#define tst_bit(adress,bit) (adress & (1<<bit))      
#define cpl_bit(adress,bit) (adress ^= (1<<bit))  

//--------------------------------------------------------------

#endif

