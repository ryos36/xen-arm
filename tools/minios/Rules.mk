%.o: %c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@
%.o: %S $(HDRS)
	$(CC) $(AFLAGS) -c $< -o $@

