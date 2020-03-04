
g++ surrogate_data.c -o surrogate_data.out
./surrogate_data.out

cd ..
for sur in `seq 0 9`;
do
	echo "surrogate $sur"
	scp ./shift_surrogates/surrogate_drivings/surr$sur.txt ./data/processed_data/driving.txt
	#calculate prc
	#first iteration
	echo -e "\tget prc"
	./prc_algorithm/get_prc.out
	scp prc_algorithm/PRC.txt result/PRC_iterations/PRC1.txt
	scp prc_algorithm/PRC_ph.txt result/PRC_iterations/PRC_ph1.txt
	#subsequent iterations
	for it in `seq 2 10`;
	do
		echo -e "\tprc improve $it"
		./prc_algorithm/improve_prc.out
		scp prc_algorithm/PRC.txt result/PRC_iterations/PRC$it.txt
		scp prc_algorithm/PRC_ph.txt result/PRC_iterations/PRC_ph$it.txt
	done
	scp prc_algorithm/PRC.txt result/PRC_harmonics.txt
	scp prc_algorithm/PRC_ph.txt result/PRC_ph.txt
	scp prc_algorithm/phase.txt result/phase.txt
	#determine error
	./error/psi_error.out
	scp ./error/ERR_psi.txt ./shift_surrogates/errors/ERR_sur$sur.txt
done
