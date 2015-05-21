FILE = qed
NAME = Qed
CREATORID = QEDi
TYPE = appl
CC = m68k-palmos-gcc
CFLAGS = -O1 -palmos5r4

#MODIFY should be defined in shell
#MODIFY = HIRES
#MODIFY = SONY
#MODIFY = HANDERA
#MODIFY = NORMPALM
#if not specified it will be HIRES
ifndef MODIFY
	MODIFY = HIRES
endif

#create asssemblerfile in compile stage -save-temps

$(FILE)_$(MODIFY).prc : grc.tmp qed_$(MODIFY).ro
#	@cp Font03e8.qed Font03e8.bin
	build-prc -o $(FILE)_$(MODIFY).prc -n $(NAME) -c $(CREATORID) -t $(TYPE) *.grc qed_$(MODIFY).ro

grc.tmp : $(FILE)_$(MODIFY)_l
	m68k-palmos-obj-res $(FILE)_$(MODIFY)_l
	@touch grc.tmp

$(FILE)_$(MODIFY)_l : $(FILE)_$(MODIFY).o
	$(CC) $(CFLAGS) $(FILE)_$(MODIFY).o -o $(FILE)_$(MODIFY)_l

$(FILE)_$(MODIFY).o : $(FILE).c $(FILE).h
	$(CC) $(CFLAGS) -D $(MODIFY) -c $(FILE).c -o $(FILE)_$(MODIFY).o

#bin.tmp : $(FILE).rcp $(FILE).h *.bmp
#bin.tmp : $(FILE).rcp $(FILE).h qed_normal/*.bmp qed_handera/*.bmp qed_hires/*.bmp
qed_$(MODIFY).ro: $(FILE).rcp $(FILE).h qed_normal/*.bmp qed_handera/*.bmp qed_hires/*.bmp
	pilrc -ro -q -allowEditID -D $(MODIFY) $(FILE).rcp
	@mv $(FILE).ro $(FILE)_$(MODIFY).ro
#	@touch bin.tmp

clean :
	rm -f *.bin *.grc *.o *.tmp $(FILE)

run :
	c:/user/palm/emulator/emulator -load_apps ../work/$(FILE).prc -run_app $(NAME) &

inst1 :
	c:/user/palm/palmclis/palmclis -uaf "Kurt 1" $(FILE).prc

inst2 :
	c:/user/palm/palmclis/palmclis -uaf "Kurt 2" $(FILE).prc

copy :
	cp -v $(FILE).* //Atlan1400/Temp
