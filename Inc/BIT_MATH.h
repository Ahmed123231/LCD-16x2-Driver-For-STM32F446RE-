#ifndef _BIT_MATH_H_
#define _BIT_MATH_H_


#define CLR_BIT(REG,BIT)     (REG&=~(1<<BIT))
#define GET_BIT(REG,BIT)     ((REG>>BIT)&0x01)
#define TOGGLE_BIT(REG,BIT)  (REG^=(1<<BIT))

#endif
