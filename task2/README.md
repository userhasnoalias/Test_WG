# Циклический буфер

## ring_buffer_var1
Плюсы:
+ Для вычисления индексов используется более быстрая операция побитового И (&) вместо взятия модуля (%)
+ Для хранения данных используется zero overhead class std::array
+ Для работы буфера нужны всего 3 целочисленных переменных (не используется дополнительная переменная для подсчета элементов)
+ Множество различных методов для чтения/записи элементов буфера

Минусы:
+ Буфер может быть только фиксированного размера 2^n
+ Класс предназначен для буферов небольшого размера (std::array располагается на стеке). Для буфера большого размера внешний
 код должен позаботиться о выделении памяти в куче.
+ Класс не thread-safe
+ Громоздкая реализация (множество методов, которые нужно протестировать)
 
## ring_buffer_var2
Плюсы:
+ Простая реализация
+ Буфер располагается в динамической памяти и может быть произвольного размера
+ Для управления памятью используются умные указатели
+ Thread-safe класс

Минусы:
+ Для вычисления индексов используется более медленная операция взятия модуля (%) нежели побитовое И (&)
+ Есть только 1 метод для чтения и 1 для записи - put и get
+ Для работы буфера требуется дополнительная переменная-счетчик count

# P.S.
Буферы также отличаются между собой по логике добавления нового элемента при переполнении.
ring_buffer_var1 отбрасывает новый элемент, если буфер полон. ring_buffer_var2 перезаписывает
самый "старый" элемент и сдвигает индекс чтения на следуюющий.
