
#compile spike determination
cd signal_thresholding
g++ signal_to_events.c -o signal_to_events.out
cd ..

#compile algorithm
cd prc_algorithm
g++ -w get_prc.c -o get_prc.out
g++ -w improve_prc.c -o improve_prc.out
cd ..

#compile error
cd error
g++ psi_error.c -o psi_error.out
g++ psi0_error.c -o psi0_error.out
cd ..
