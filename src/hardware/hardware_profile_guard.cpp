#if !defined(HARDWARE_MASTER_NO_RTC) && !defined(HARDWARE_RTC_LOCAL)
#error "No hardware profile selected. Set HARDWARE_PROFILE in config.env to MASTER_NO_RTC or RTC_LOCAL."
#endif

#if defined(HARDWARE_MASTER_NO_RTC) && defined(HARDWARE_RTC_LOCAL)
#error "Multiple hardware profiles selected. Set only one HARDWARE_PROFILE in config.env."
#endif
