
echo -e "--- Dodawanie ---"
./compiler test.mr < testy3/add.imp && int/int-cln test.mr

echo -e "--- Odejmowanie ---"
./compiler test.mr < testy3/sub.imp && int/int-cln test.mr

echo -e "--- Mnożenie ---"
./compiler test.mr < testy3/mult.imp && int/int-cln test.mr

echo -e "--- Dzielenie ---"
./compiler test.mr < testy3/div.imp && int/int-cln test.mr

echo -e "--- Różne testy ---"
./compiler test.mr < testy3/mixed.imp && int/int-cln test.mr
