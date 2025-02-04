FIRMWARE ?= step0_leds
CABILLOT_FIRMWARE ?= step9_cabillot
BOARD ?= ulx3s

.PHONY: all clean reprog

cabillot: cabillot.si
	make -C firmware $(CABILLOT_FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

reprog: 
	silice-make.py --reprogram -s cabillot.si -b ulx3s -p basic,audio,oled,buttons,sdcard -o BUILD_cabillot

step0: step0.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step0.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si > $@

step1: step1.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step1.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED > $@

step2: step2.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step2.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,SD > $@

step3: step3.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step3.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,SD > $@

step4: step4.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step4.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,BTN,SD > $@

step5: step5.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step5.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,BTN,SD,STREAM > $@

step6: step6.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step6.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,BTN,SD,STREAM > $@

step7: step7.si
	make -C firmware $(FIRMWARE) DEFINES="-DHWFBUFFER"
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

step7.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,BTN,SD,STREAM,HWFBUFFER > $@

final: final.si
	make -C firmware $(FIRMWARE)
ifeq ($(BOARD),verilator)
	silice-make.py -s $@.si -b $(BOARD) -p basic,oled -o BUILD_$(subst :,_,$@) $(ARGS)
else
	silice-make.py -s $@.si -b $(BOARD) -p basic,audio,oled,buttons,sdcard -o BUILD_$(subst :,_,$@) $(ARGS)
endif

final.si : hardware/main.si
	python ../tools/solutions.py -i hardware/main.si -s LED,AUDIO,SCREEN,BTN,SD,STREAM,HWFBUFFER,PWM > $@

clean:
	make -C firmware clean
	rm -rf BUILD_*
	rm -f *.lpp
