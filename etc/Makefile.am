# The directory where the data files will be installed

install-data-local:
	if [ ! -d $(prefix)/etc ]; then $(INSTALL) -d $(prefix)/etc; fi

##if MGLIB_MVFB
##	$(INSTALL_DATA) $(abs_top_srcdir)/etc/mvfb.cfg $(prefix)/etc/mvfb.cfg
##else
##	$(INSTALL_DATA) $(abs_top_srcdir)/etc/MiniGUI.cfg $(prefix)/etc/MiniGUI.cfg
##endif	

miniguicfgdir = $(prefix)/etc
if MGLIB_MVFB
miniguicfg_DATA = $(abs_top_srcdir)/etc/mvfb.cfg
else
miniguicfg_DATA = $(abs_top_srcdir)/etc/MiniGUI.cfg
endif	

EXTRA_DIST = MiniGUI.cfg mvfb.cfg

