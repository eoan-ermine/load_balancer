.PHONY: format tidy

format:
	find ./ -iname *.hpp -o -iname *.cpp | xargs clang-format -i
tidy:
	cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=on
	clang-tidy -p build/ -header-filter=. `find ./ -iname *.cpp`
