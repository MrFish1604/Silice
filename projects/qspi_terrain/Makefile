SERIAL_PORT ?= COM13

icestick: tt_fpga_main.si
	silice-make.py -s tt_fpga_main.si -b tinytapeout -t tt_icestick -p basic -o BUILD_$(subst :,_,$@) -a force-reset-init,split-inouts $(ARGS)

tinytapeout: tt_main.si data.raw
	silice-make.py -s tt_main.si -b tinytapeout -t tinytapeout -p basic -o BUILD_$(subst :,_,$@) -a force-reset-init,split-inouts $(ARGS)

verilator: tt_sim_main.si data.raw
		silice-make.py -s tt_sim_main.si -b $@ -p basic,spiscreen -o BUILD_$(subst :,_,$@) -t shell  -a force-reset-init,split-inouts $(ARGS)

clean:
	rm -rf BUILD_*

data.raw: make_data.si
		silice-make.py -s make_data.si -b verilator -p basic -o BUILD_$(subst :,_,$@) $(ARGS)

program_data: data.raw
		iceprog ../qpsram/bitstreams/icestick.bin
		sleep 1
		python  ../qpsram/xfer.py $(SERIAL_PORT) w 0 data.raw
