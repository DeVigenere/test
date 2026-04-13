#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>



struct ListNode { // Задание не менять
	ListNode* prev = nullptr;
	ListNode* next = nullptr;
	ListNode* rand = nullptr;
	std::string data;
};

class Serial { //создал класс сериализации
public:
	static ListNode* Read(const std::string& namefile) { //читаем из файла и возвращаем указатель листа
		std::ifstream in(namefile);
		
		if (!in.is_open()) { // если не открывается кидаем исключение
			throw std::runtime_error("Cannot open!");
		}
		
		std::vector<std::string> data_list{}; //до точки запятой
		std::vector<int> rand{}; // после
		std::string line{}; // вся строчка
		
		while (std::getline(in, line)) { 
			if (line.empty()) {
				continue;
			}
			
			size_t dot_and_zapyatya = line.find(';'); //ищем где находится точка с запятой, чтобы дальше разделять текст и индекс
			
			std::string data_text = line.substr(0, dot_and_zapyatya); //читаем до точки с запятой
			std::string index = line.substr(dot_and_zapyatya + 1); // после точки с запятой
			
			int rand_index = std::stoi(index); //явное преобразование типа
			
			data_list.push_back(data_text); 
			rand.push_back(rand_index); 
		}
		size_t N = data_list.size(); // узнаём размер списка, если его нет, то возвращаем nullptr
		if (N == 0) {
			return nullptr;
		}
		std::vector<ListNode*> nodes(N); // вектор, где все элементы это тип данных, который мы создали, соответственно все элементы будут обладать полями
		
		for (size_t i = 0; i < N; ++i) { // в каждом элементе вектор создаю в кучу листнод и в его дату заношу то, что из файла спарсили
			nodes[i] = new ListNode();
			nodes[i]->data = data_list[i];
		}
		for (size_t i = 0; i < N; ++i) { // и каждому элементу вектора даю указатель на предыдущий и следующий элемент
			if (i > 0) {
				nodes[i]->prev = nodes[i - 1];
			}
			if (i < N - 1) {
				nodes[i]->next = nodes[i + 1];
			}
		}

		// использую префиксный инкремент чтобы сразу изменялся i, а не через копирование
		
		for (size_t i = 0; i < N; ++i) { //восстанавливаем значения указателей
			if (rand[i] >= 0 && rand[i] < static_cast<int>(N)) {
				nodes[i]->rand = nodes[rand[i]];
			}
			else {
				nodes[i]->rand = nullptr;
			}
		}
		return nodes[0];

	}
	static void SerialtoBinary(ListNode* head, const std::string& namefile) { 
		std::ofstream out(namefile, std::ios::binary);
		
		if (!out.is_open()) {
			throw std::runtime_error("Cannot open output!");
		}
		std::vector<ListNode*> nodes;
		
		
		uint64_t index = 0;
		for (ListNode* curr = head; curr != nullptr; curr = curr->next) { 
			nodes.push_back(curr);
		}
		uint64_t N = nodes.size();

		out.write(reinterpret_cast<const char*>(&N), sizeof(N)); //записываем бинарно значение N
		
		for (ListNode* node : nodes) { //пробегаемся по коллекции
			uint64_t data_len = node->data.size();
			out.write(reinterpret_cast<const char*>(&data_len), sizeof(data_len));
			if (data_len > 0) {
				out.write(node->data.c_str(), data_len);
			}
		}
		
		for (size_t i = 0; i < N; ++i) {
			int64_t rand_index = -1;
			if (nodes[i]->rand != nullptr) {
				for (size_t j = 0; j < N; ++j) {
					if (nodes[j] == nodes[i]->rand) {
						rand_index = static_cast<int64_t>(j);
						break;
					}
				}
			}
			out.write(reinterpret_cast<const char*>(&rand_index), sizeof(rand_index));
		}
	}
	static void deleteList(ListNode* head) {
		ListNode* curr = head;
		while (curr != nullptr) { // очищаем кучу до тех пор, пока не дойдём до nullptr
			ListNode* next = curr->next;
			delete curr;
			curr = next;
		}
	}

};


int main() {
	
	ListNode* head = Serial::Read("inlet.in");
	if (head == nullptr) {
		std::cout << "Empty list!" << std::endl;
		return 0;
	}
	std::cout << "List get info from inlet!" << std::endl;

	Serial::SerialtoBinary(head, "outlet.out");
	std::cout << "serial in outlet" << std::endl;

	Serial::deleteList(head);
	return 0;
}