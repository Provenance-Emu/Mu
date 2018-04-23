//basic accessors
static inline uint8_t registerArrayRead8(uint32_t address){return BUFFER_READ_8(palmReg, address, 0, 0xFFF);}
static inline uint16_t registerArrayRead16(uint32_t address){return BUFFER_READ_16(palmReg, address, 0, 0xFFF);}
static inline uint32_t registerArrayRead32(uint32_t address){return BUFFER_READ_32(palmReg, address, 0, 0xFFF);}
static inline void registerArrayWrite8(uint32_t address, uint8_t value){BUFFER_WRITE_8(palmReg, address, 0, 0xFFF, value);}
static inline void registerArrayWrite16(uint32_t address, uint16_t value){BUFFER_WRITE_16(palmReg, address, 0, 0xFFF, value);}
static inline void registerArrayWrite32(uint32_t address, uint32_t value){BUFFER_WRITE_32(palmReg, address, 0, 0xFFF, value);}

//register setters
static inline void setIprIsrBit(uint32_t interruptBit){
   //allows for setting an interrupt with masking by IMR and logging in IPR
   registerArrayWrite32(IPR, registerArrayRead32(IPR) | interruptBit);
   registerArrayWrite32(ISR, registerArrayRead32(ISR) | (interruptBit & ~registerArrayRead32(IMR)));
}

static inline void clearIprIsrBit(uint32_t interruptBit){
   registerArrayWrite32(IPR, registerArrayRead32(IPR) & ~interruptBit);
   registerArrayWrite32(ISR, registerArrayRead32(ISR) & ~interruptBit);
}

static inline void setCsa(uint16_t value){
   chips[CHIP_A_ROM].enable = CAST_TO_BOOL(value & 0x0001);
   chips[CHIP_A_ROM].readOnly = CAST_TO_BOOL(value & 0x8000);
   chips[CHIP_A_ROM].size = 0x20000/*128kb*/ << (value >> 1 & 0x0007);

   //CSA is now just a normal chipselect
   if(chips[CHIP_A_ROM].enable && chips[CHIP_A_ROM].inBootMode)
      chips[CHIP_A_ROM].inBootMode = false;

   registerArrayWrite16(CSA, value & 0x81FF);
}

static inline void setCsb(uint16_t value){
   uint16_t csControl1 = registerArrayRead16(CSCTRL1);

   chips[CHIP_B_SED].enable = CAST_TO_BOOL(value & 0x0001);
   chips[CHIP_B_SED].readOnly = CAST_TO_BOOL(value & 0x8000);
   chips[CHIP_B_SED].size = 0x20000/*128kb*/ << (value >> 1 & 0x0007);

   //attributes
   chips[CHIP_B_SED].supervisorOnlyProtectedMemory = CAST_TO_BOOL(value & 0x4000);
   chips[CHIP_B_SED].readOnlyForProtectedMemory = CAST_TO_BOOL(value & 0x2000);
   if(csControl1 & 0x4000 && csControl1 & 0x0001)
      chips[CHIP_B_SED].unprotectedSize = 0x8000/*32kb*/ << ((value >> 11 & 0x0003) | 0x0004);
   else
      chips[CHIP_B_SED].unprotectedSize = 0x8000/*32kb*/ << (value >> 11 & 0x0003);

   registerArrayWrite16(CSB, value & 0xF9FF);
}

static inline void setCsc(uint16_t value){
   uint16_t csControl1 = registerArrayRead16(CSCTRL1);

   chips[CHIP_C_USB].enable = CAST_TO_BOOL(value & 0x0001);
   chips[CHIP_C_USB].readOnly = CAST_TO_BOOL(value & 0x8000);
   chips[CHIP_C_USB].size = 0x8000/*32kb*/ << (value >> 1 & 0x0007);

   //attributes
   chips[CHIP_C_USB].supervisorOnlyProtectedMemory = CAST_TO_BOOL(value & 0x4000);
   chips[CHIP_C_USB].readOnlyForProtectedMemory = CAST_TO_BOOL(value & 0x2000);
   if(csControl1 & 0x4000 && csControl1 & 0x0004)
      chips[CHIP_C_USB].unprotectedSize = 0x8000/*32kb*/ << ((value >> 11 & 0x0003) | 0x0004);
   else
      chips[CHIP_C_USB].unprotectedSize = 0x8000/*32kb*/ << (value >> 11 & 0x0003);

   registerArrayWrite16(CSC, value & 0xF9FF);
}

static inline void setCsd(uint16_t value){
   uint16_t csControl1 = registerArrayRead16(CSCTRL1);

   chips[CHIP_D_RAM].enable = CAST_TO_BOOL(value & 0x0001);
   chips[CHIP_D_RAM].readOnly = CAST_TO_BOOL(value & 0x8000);
   if(csControl1 & 0x0040 && value & 0x0200)
      chips[CHIP_D_RAM].size = 0x800000/*8mb*/ << (value >> 1 & 0x0001);
   else
      chips[CHIP_D_RAM].size = 0x8000/*32kb*/ << (value >> 1 & 0x0007);

   //attributes
   chips[CHIP_D_RAM].supervisorOnlyProtectedMemory = CAST_TO_BOOL(value & 0x4000);
   chips[CHIP_D_RAM].readOnlyForProtectedMemory = CAST_TO_BOOL(value & 0x2000);
   if(csControl1 & 0x4000 && csControl1 & 0x0010)
      chips[CHIP_D_RAM].unprotectedSize = 0x8000/*32kb*/ << ((value >> 11 & 0x0003) | 0x0004);
   else
      chips[CHIP_D_RAM].unprotectedSize = 0x8000/*32kb*/ << (value >> 11 & 0x0003);

   registerArrayWrite16(CSD, value);
}

static inline void setCsgba(uint16_t value){
   uint16_t csugba = registerArrayRead16(CSUGBA);

   //add extra address bits if enabled
   if(csugba & 0x8000)
      chips[CHIP_A_ROM].start = (csugba >> 12 & 0x0007) << 29 | value >> 1 << 14;
   else
      chips[CHIP_A_ROM].start = value >> 1 << 14;

   registerArrayWrite16(CSGBA, value & 0xFFFE);
}

static inline void setCsgbb(uint16_t value){
   uint16_t csugba = registerArrayRead16(CSUGBA);

   //add extra address bits if enabled
   if(csugba & 0x8000)
      chips[CHIP_B_SED].start = (csugba >> 8 & 0x0007) << 29 | value >> 1 << 14;
   else
      chips[CHIP_B_SED].start = value >> 1 << 14;

   registerArrayWrite16(CSGBB, value & 0xFFFE);
}

static inline void setCsgbc(uint16_t value){
   uint16_t csugba = registerArrayRead16(CSUGBA);

   //add extra address bits if enabled
   if(csugba & 0x8000)
      chips[CHIP_C_USB].start = (csugba >> 4 & 0x0007) << 29 | value >> 1 << 14;
   else
      chips[CHIP_C_USB].start = value >> 1 << 14;

   registerArrayWrite16(CSGBC, value & 0xFFFE);
}

static inline void setCsgbd(uint16_t value){
   uint16_t csugba = registerArrayRead16(CSUGBA);

   //add extra address bits if enabled
   if(csugba & 0x8000)
      chips[CHIP_D_RAM].start = (csugba & 0x0007) << 29 | value >> 1 << 14;
   else
      chips[CHIP_D_RAM].start = value >> 1 << 14;

   registerArrayWrite16(CSGBD, value & 0xFFFE);
}

static inline void setCsctrl1(uint16_t value){
   uint16_t oldCsctrl1 = registerArrayRead16(CSCTRL1);

   registerArrayWrite16(CSCTRL1, value & 0x7F55);
   if((oldCsctrl1 & 0x4055) != (value & 0x4055)){
      //something important changed, update all chipselects
      //CSA is not dependant on CSCTRL1
      setCsb(registerArrayRead16(CSB));
      setCsc(registerArrayRead16(CSC));
      setCsd(registerArrayRead16(CSD));
   }
}

//csctrl 2 and 3 only deal with timing and bus transfer size

static inline void setPllfsr16(uint16_t value){
   uint16_t oldPllfsr = registerArrayRead16(PLLFSR);
   if(!(oldPllfsr & 0x4000)){
      //frequency protect bit not set
      registerArrayWrite16(PLLFSR, (value & 0x4FFF) | (oldPllfsr & 0x8000));//preserve CLK32 bit
      double prescaler1 = (registerArrayRead16(PLLCR) & 0x0080) ? 2.0 : 1.0;
      double p = value & 0x00FF;
      double q = (value & 0x0F00) >> 8;
      palmCrystalCycles = 2.0 * (14.0 * (p + 1.0) + q + 1.0) / prescaler1;
      debugLog("New CPU frequency of:%f cycles per second.\n", CPU_FREQUENCY);
      debugLog("New CLK32 cycle count of:%f.\n", palmCrystalCycles);
   }
}

static inline void setPllcr(uint16_t value){
   //values that matter are disable PLL, prescaler 1 and possibly wakeselect
   registerArrayWrite16(PLLCR, value & 0x3FBB);
   uint16_t pllfsr = registerArrayRead16(PLLFSR);
   double prescaler1 = (value & 0x0080) ? 2.0 : 1.0;
   double p = pllfsr & 0x00FF;
   double q = (pllfsr & 0x0F00) >> 8;
   palmCrystalCycles = 2.0 * (14.0 * (p + 1.0) + q + 1.0) / prescaler1;
   debugLog("New CPU frequency of:%f cycles per second.\n", CPU_FREQUENCY);
   debugLog("New CLK32 cycle count of:%f.\n", palmCrystalCycles);

   if(value & 0x0008){
      //The PLL shuts down 30 clock cycles of SYSCLK after the DISPLL bit is set in the PLLCR
      m68k_modify_timeslice(-m68k_cycles_remaining() + 30);
      debugLog("Disable PLL set, CPU off in 30 cycles!\n");
   }
}

static inline void setScr(uint8_t value){
   uint8_t oldScr = registerArrayRead8(SCR);
   uint8_t newScr = value;

   //preserve privilege violation, write protect violation and bus error timeout
   newScr |= oldScr & 0xE0;

   //clear violations on writing 1 to them
   newScr &= ~(oldScr & value & 0xE0);

   chips[CHIP_REGISTERS].supervisorOnlyProtectedMemory = CAST_TO_BOOL(value & 0x08);

   registerArrayWrite8(SCR, newScr);//must be written before calling setRegisterFFFFAccessMode
   if((newScr & 0x04) != (oldScr & 0x04)){
      if(newScr & 0x04)
         setRegisterXXFFAccessMode();
      else
         setRegisterFFFFAccessMode();
   }
}

static inline void setIlcr(uint16_t value){
   uint16_t oldIlcr = registerArrayRead16(ILCR);
   uint16_t newIlcr = 0;

   //SPI1
   if((value & 0x7000) != 0x0000 && (value & 0x7000) != 0x7000)
      newIlcr |= value & 0x7000;
   else
      newIlcr |= oldIlcr & 0x7000;

   //UART2
   if((value & 0x0700) != 0x0000 && (value & 0x0700) != 0x0700)
      newIlcr |= value & 0x0700;
   else
      newIlcr |= oldIlcr & 0x0700;

   //PWM2
   if((value & 0x0070) != 0x0000 && (value & 0x0070) != 0x0070)
      newIlcr |= value & 0x0070;
   else
      newIlcr |= oldIlcr & 0x0070;

   //TMR2
   if((value & 0x0007) != 0x0000 && (value & 0x0007) != 0x0007)
      newIlcr |= value & 0x0007;
   else
      newIlcr |= oldIlcr & 0x0007;
}

//register getters
static inline uint8_t getPortDValue(){
   uint8_t requestedRow = registerArrayRead8(PKDIR) & registerArrayRead8(PKDATA);//keys are requested on port k and read on port d
   uint8_t portDValue = 0x00;//ports always read the chip pins even if they are set to output
   uint8_t portDData = registerArrayRead8(PDDATA);
   uint8_t portDDir = registerArrayRead8(PDDIR);
   uint8_t portDPolarity = registerArrayRead8(PDPOL);

   portDValue |= 0x80;//battery not dead bit

   if(!palmSdCard.inserted){
      portDValue |= 0x20;
   }

   if((requestedRow & 0x20) == 0){
      //kbd row 0, pins are 0 when button pressed and 1 when released, Palm OS then uses PDPOL to swap back to pressed == 1
      portDValue |= !palmInput.buttonCalender | !palmInput.buttonAddress << 1 | !palmInput.buttonTodo << 2 | !palmInput.buttonNotes << 3;
   }

   if((requestedRow & 0x40) == 0){
      //kbd row 1, pins are 0 when button pressed and 1 when released, Palm OS then uses PDPOL to swap back to pressed == 1
      portDValue |= !palmInput.buttonUp | !palmInput.buttonDown << 1;
   }

   if((requestedRow & 0x80) == 0){
      //kbd row 2, pins are 0 when button pressed and 1 when released, Palm OS then uses PDPOL to swap back to pressed == 1
      portDValue |= !palmInput.buttonPower | !palmInput.buttonContrast << 1 | !palmInput.buttonAddress << 3;
   }

   portDValue |= 0x50;//floating pins are high
   portDValue ^= portDPolarity;//only input polarity is affected by PDPOL
   portDValue &= ~portDDir;//only use above pin values for inputs
   portDValue |= portDData & portDDir;//if a pin is an output and has its data bit set return that too

   return portDValue;
}

static inline uint8_t getPortKValue(){
   uint8_t portKValue = 0x00;//ports always read the chip pins even if they are set to output
   uint8_t portKData = registerArrayRead8(PKDATA);
   uint8_t portKDir = registerArrayRead8(PKDIR);
   uint8_t portKSel = registerArrayRead8(PKSEL);

   portKValue |= !palmMisc.inDock << 2;
   portKValue |= 0xFB;//floating pins are high
   portKValue &= ~portKDir & portKSel;
   portKValue |= portKData & portKDir & portKSel;

   return portKValue;
}
