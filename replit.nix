{ pkgs }: {
	deps = [
   pkgs.valgrind
		pkgs.clang
		pkgs.ccls
		pkgs.gdb
		pkgs.gnumake
	];
}