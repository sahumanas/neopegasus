################################################################################
##
## An ugly trick is used here to overcome a bug in g++ 2.9.5. G++ failes to
## cleanup object files that are placed in directories other than the
## current one (using the -o option). To overcome this bug, we use -o.tmp.o 
## and then move the file to the object directory.
##
################################################################################

_TMP_O = .tmp.o
$(OBJ_DIR)/%.o: %.cpp $(ERROR)
	$(CXX) -c -o$(_TMP_O) $(FLAGS) $(GUARDIAN_FLAGS) $(LOCAL_DEFINES) $(DEFINES) $(SYS_INCLUDES) $(INCLUDES) $*.cpp
	$(COPY) $(_TMP_O) $@
	$(RM) $(_TMP_O)
	@ $(ECHO)
