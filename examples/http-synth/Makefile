all: http-synth.galileo.efi
CONTIKI=../..
APPS = webserver-nano

CONTIKI_WITH_IPV4=1

# See cpu/x86/mm/README.md and platform/galileo/README.md for more information
# on the following two options.
X86_CONF_PROT_DOMAINS=tss
X86_CONF_RESTRICT_DMA=1

CFLAGS += -DWEBSERVER_CONF_NANO=0

include $(CONTIKI)/Makefile.include