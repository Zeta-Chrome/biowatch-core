#include "flash.h"
#include "lib/utils.h"
#include "stm32wb55xx.h"

#define KEY1 0x45670123
#define KEY2 0xCDEF89AB
#define FLASH_FLAG_SR_ERRORS                                                                   \
	(FLASH_SR_OPERR | FLASH_SR_PROGERR | FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_SIZERR | \
	 FLASH_SR_PGSERR | FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_RDERR | FLASH_SR_OPTVERR)

void flash_unlock()
{
	if (CHECK_BIT(FLASH->CR, FLASH_CR_LOCK_Msk)) {
		WRITE_FIELD(FLASH->KEYR, KEY1);
		__ISB();
		WRITE_FIELD(FLASH->KEYR, KEY2);
	}
}

void flash_lock()
{
	SET_FIELD(FLASH->CR, FLASH_CR_LOCK_Msk);
}

static inline void flash_wait_ready()
{
	while (CHECK_BIT(FLASH->SR, (FLASH_SR_BSY | FLASH_SR_PESD)))
		;
}

void flash_erase_page(uint8_t page_no)
{
	flash_wait_ready();
	WRITE_FIELD(FLASH->SR, FLASH_FLAG_SR_ERRORS);
	SET_FIELD(FLASH->CR, FLASH_CR_PER_Msk);
	MODIFY_FIELD(FLASH->CR, FLASH_CR_PNB_Msk, FLASH_CR_PNB_Pos, page_no);
	SET_FIELD(FLASH->CR, FLASH_CR_STRT_Msk);
	flash_wait_ready();
	CLEAR_FIELD(FLASH->CR, FLASH_CR_PER_Msk);
}

void flash_mass_erase()
{
	flash_wait_ready();
	SET_FIELD(FLASH->SR, FLASH_FLAG_SR_ERRORS);
	SET_FIELD(FLASH->CR, FLASH_CR_MER_Msk);
	SET_FIELD(FLASH->CR, FLASH_CR_STRT_Msk);
	flash_wait_ready();
	CLEAR_FIELD(FLASH->CR, FLASH_CR_MER_Msk);
}

static void flash_double_word(uint32_t dst_addr, const uint64_t *data)
{
	flash_wait_ready();
	SET_FIELD(FLASH->SR, FLASH_FLAG_SR_ERRORS);
	SET_FIELD(FLASH->CR, FLASH_CR_PG_Msk);
	*(volatile uint32_t *)dst_addr = *((uint32_t *)data);
	*((volatile uint32_t *)dst_addr + 1) = *((uint32_t *)data + 1);
	flash_wait_ready();
	if (CHECK_BIT(FLASH->SR, FLASH_SR_EOP_Msk))
		SET_BIT(FLASH->SR, FLASH_SR_EOP_Msk);
	else
		bw_error_handler();
}

void flash_program(uint32_t dst_addr, const uint64_t *data, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
		flash_double_word(dst_addr + 8 * i, data + i);

	CLEAR_FIELD(FLASH->CR, FLASH_CR_PG_Msk);
}

void flash_configure_latency(uint32_t freq)
{
	uint8_t latency = 0;
	if (freq <= 18000000)
		latency = 0;
	else if (freq <= 36000000)
		latency = 1;
	else if (freq <= 54000000)
		latency = 2;
	else if (freq <= 64000000)
		latency = 3;

	MODIFY_FIELD(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_Pos, latency);
	while (READ_FIELD(FLASH->ACR, FLASH_ACR_LATENCY_Msk, FLASH_ACR_LATENCY_Pos) != latency)
		;
}
