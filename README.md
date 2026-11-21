использование: unu [опции] <картинка>
  -i, --input <>файл    что превращать(image.png)
  -o, --output <файл>   куда писать результат (out.png)
      --font <файл>     путь к .ttf
      --colours <файл>  таблица цветов (.txt)
  -m, --mult <1..31>    ширина блока в пикселях (4)
      --px <8..128>     размер шрифта (32)
      --cyrillic        добавить кириллицу
      --text            выводить текст, а не картинку
  -h, --help            эта справка
  -bg --background      установить цвет фона за символами (rgb) например "255 255 255"

  Работает, беря квадраты со стороной m в изображении и приводит к такому размеру данный шрифт. После этого ищет наименьшее расстояние банальным средним арифметическим
  Аналогичным образом выбирает ближайший цвет через теорему Пифагора в пространстве OKLab. 
  Код писался на пару раз и может быть плохо оптимизированным)

Пример: 

<img width="736" height="1104" alt="Just white truck" src="https://github.com/user-attachments/assets/66916320-2382-472e-b34c-edf059a5aa19" />

[Моя фотография, https://pin.it/4OfmRst7k]

<img width="3680" height="5809" alt="out" src="https://github.com/user-attachments/assets/7aff8db9-23ac-4196-adb5-bab568aa990b" />

С шрифтом DejaVuSansMono и параметром m = 4

<img width="736" height="1104" alt="Venice" src="https://github.com/user-attachments/assets/acbd2a4d-3baf-4743-a751-882ea3a6b788" />

[Тоже моя фотография, пожалуйста, не судите за свои же изображения >>]

<img width="3680" height="5809" alt="out" src="https://github.com/user-attachments/assets/a6e2c91f-b4bb-4a36-a39e-8c2ac795ce7d" />

С аналогичными параметрами
