echo "=============ESECUZIONE AUTOMATICA AVVIATA==============="
echo "=============Primo step=================================="
./clean.sh
cp config.fcc config.0
cp input_begin.dat input.dat
./MolDyn_NVE.exe
cp input_equil.dat input.dat

cp config.final config.0
cp old.final old.0

for i in `seq 1 5`;
do
echo "==============Tentativo " $i "========================"
./MolDyn_NVE.exe
cp config.final config.0
cp old.final old.0
done

cp output_temp.dat ist_T.dat

echo "============== Esecuzione delle misure ==============="

cp input_mis.dat input.dat
./clean.sh
./MolDyn_NVE.exe
