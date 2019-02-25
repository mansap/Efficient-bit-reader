target = code_challenge

$(target): $(target).c
	gcc -o $(target) $(target).c

run: $(target)
	./$(target) ${ARGS}

clean:
	$(RM) $(target)