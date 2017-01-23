
echo -e "--- Dodawanie ---"
./compiler test.mr < testy3/add.imp && interpreter/interpreter-cln test.mr

echo -e "--- Odejmowanie ---"
./compiler test.mr < testy3/sub.imp && interpreter/interpreter-cln test.mr

echo -e "--- Mnożenie ---"
./compiler test.mr < testy3/mult.imp && interpreter/interpreter-cln test.mr

echo -e "--- Różne testy ---"
./compiler test.mr < testy3/mixed.imp && interpreter/interpreter-cln test.mr
