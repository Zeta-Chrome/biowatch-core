#ifndef HAL_REG_H
#define HAL_REG_H

#include <stdint.h>

static inline uint8_t reg_get_bit(volatile uint32_t *reg, uint8_t pos)
{
    return (*reg >> pos) & 1U;
}

static inline uint32_t reg_get_field(volatile uint32_t *reg, uint8_t pos, uint8_t width)
{
    uint32_t mask = (1UL << width) - 1UL;
    return (*reg >> pos) & mask;
}

static inline void reg_clear_bit(volatile uint32_t *reg, uint8_t pos)
{
    *reg &= ~(1UL << pos);
}

static inline void reg_set_bit(volatile uint32_t *reg, uint8_t pos)
{
    *reg |= 1UL << pos;
}

static inline void reg_set_field(volatile uint32_t *reg, uint8_t pos, uint8_t width, uint32_t val)
{
    uint32_t mask = ((1UL << width) - 1UL) << pos;
    *reg = (*reg & ~mask) | ((val << pos) & mask);
}

static inline void reg_set_mask(volatile uint32_t *reg, uint32_t mask)
{
    *reg |= mask;
}

static inline void reg_clear_mask(volatile uint32_t *reg, uint32_t mask)
{
    *reg &= ~mask;
}

#endif
