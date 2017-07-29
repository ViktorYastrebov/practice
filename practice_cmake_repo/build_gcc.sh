if [[ ! -e ./objects ]]; then
	mkdir ./objects
fi
cd objects
cmake -G"Unix Makefiles" ..

