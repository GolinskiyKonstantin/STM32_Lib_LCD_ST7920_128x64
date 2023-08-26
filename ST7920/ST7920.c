/*

  ******************************************************************************
  * @file 			( фаил ):   ST7920.c
  * @brief 		( описание ):  	
  ******************************************************************************
  * @attention 	( внимание ):
  ******************************************************************************
  
*/

/* Includes ----------------------------------------------------------*/
#include "ST7920.h"

// буфер кадра
uint8_t ST7920_buffer[SCREEN_WIDTH * SCREEN_HEIGHT / 8];

// переменная для хранения ротации экрана, 0- поворот на 0 градусов ( по умолчанию ), 1- поворот на 180 градусов
uint8_t rotation = 0;

//==============================================================================
// Процедура управления SPI
//==============================================================================
void ST7920_CS_reset(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
}
//==============================================================================

//==============================================================================
// Процедура управления SPI
//==============================================================================
void ST7920_CS_set(void) {
	HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
}
//==============================================================================

/*
	******************************************************************************
	* @brief	 ( описание ):
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_RST_reset( void ){
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):
	* @param	( параметры ):
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_RST_set( void ){
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  отправка данных в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_w_dat( uint8_t Dat ){
	
	ST7920_CS_set();

	uint8_t  tx_buffer = 0xFA; 			//Отправка данных. RW = 0/RS = 1
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};

	tx_buffer = Dat & 0xF0; 			//Разбиваем 8 бит на 2 части. Передаем 7-4 бит.
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};
	
	tx_buffer = Dat << 4; 				//Разбиваем 8 бит на 2 части. Передаем оставшиеся 3-0 бит.
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};
		 
	ST7920_CS_reset();
}  
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  отправка команды в SPI
	* @param	( параметры ):	1 байт информации
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_w_cmd( uint8_t Command ){
	
	ST7920_CS_set();

	uint8_t  tx_buffer = 0xF8; 					//Отправка команды. RW = 0/RS = 0
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};

	tx_buffer = Command & 0xF0; 				//Разбиваем 8 бит на 2 части. Передаем 7-4 бит.
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};

	tx_buffer = Command << 4; 					//Разбиваем 8 бит на 2 части. Передаем оставшиеся 3-0 бит.
	HAL_SPI_Transmit(&ST7920_SPI_HAL, &tx_buffer, 1, HAL_MAX_DELAY);
	while(HAL_SPI_GetState(&ST7920_SPI_HAL) != HAL_SPI_STATE_READY){};
	
	ST7920_CS_reset();
} 
//--------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  инициализация дисплея ( при другой инициализации можно поменять параметры внутри функции )
	* @param	( параметры ):	
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Init( void ){
	
	HAL_Delay(100);

	ST7920_RST_reset(); 	//Дернем ножку RST
	HAL_Delay(10);
	ST7920_RST_set();
	HAL_Delay(40); 			//Ждем 40 мс

	//Далее все согласно Datasheet://
	uint8_t tx_buffer = 0x30; //Function set 8-BIT interface
	ST7920_w_cmd(tx_buffer);
	HAL_Delay(1);
	ST7920_w_cmd(tx_buffer);
	HAL_Delay(1);
	tx_buffer = 0x0C; //D = 1, C = 0, B = 0.	DISPLAY ON. cursor off. blink off
	ST7920_w_cmd(tx_buffer);
	HAL_Delay(1);
	tx_buffer = 0x01;//Display Clean
	ST7920_w_cmd(tx_buffer);
	HAL_Delay(12);
	tx_buffer = 0x06;//Cursor increment right no shift
	ST7920_w_cmd(tx_buffer);
	HAL_Delay(100);
   
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция включения/выключения графического режима
	* @param	( параметры ): 1-enable, 0-disable
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Graphic_mode(bool enable)   // 1-enable, 0-disable
{
	if (enable) {
		ST7920_w_cmd(0x34);  // Т.к. работаем в 8мибитном режиме, то выбираем 0x30 + RE = 1. Переходим в extended instruction.
		HAL_Delay(1);
		ST7920_w_cmd(0x36);  // Включаем графический режим
		HAL_Delay(1);
	}

	else if (!enable) {
		ST7920_w_cmd(0x30);  // Т.к. работаем в 8мибитном режиме, то выбираем 0x30 + RE = 0. Переходим в basic instruction.
		HAL_Delay(1);
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Display ON OFF ( очищаем экран но не очищаем буфер кадра )
	* @param	( параметры ): 1-enable, 0-disable
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_Display_On_Off(bool enable)   // 1-enable, 0-disable
{
	if (enable) {
		ST7920_Update();
	}
	else if (!enable) {
		uint8_t x, y;
		for (y = 0; y < 64; y++) {
			if (y < 32) {
				ST7920_w_cmd(0x80 | y);
				ST7920_w_cmd(0x80);
			} else {
				ST7920_w_cmd(0x80 | (y - 32));
				ST7920_w_cmd(0x88);
			}
			for (x = 0; x < 8; x++) {
				ST7920_w_dat(0x00);
				ST7920_w_dat(0x00);
			}
		}
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция очистки дисплея в графическом режиме
	* @param	( параметры ):
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_Clear( void ) {
/// Функция очистки дисплея в графическом режиме
	uint8_t x, y;
	for (y = 0; y < 64; y++) {
		if (y < 32) {
			ST7920_w_cmd(0x80 | y);
			ST7920_w_cmd(0x80);
		} else {
			ST7920_w_cmd(0x80 | (y - 32));
			ST7920_w_cmd(0x88);
		}
		for (x = 0; x < 8; x++) {
			ST7920_w_dat(0x00);
			ST7920_w_dat(0x00);
		}
	}
	ST7920_Clear_frame_buffer();
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  функция рисует монохромныую картинку
	* @param	( параметры ):	начальные координаты X Y, сама картинка, ширина и высота картинки и цвет
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint8_t color){

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++){
        for(int16_t i=0; i<w; i++){
            if(i & 7){

               byte <<= 1;
            }
            else{
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80){
            	if(color){
            		ST7920_Draw_pixel(x+i, y);
            	}
            	else{
            		ST7920_Clear_pixel(x+i, y);
            	}
            }
        }
    }
}
//----------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция рисования пикселя на экране
	* @param	( параметры ): координаты X(от 0 до 127) и Y(от 0 до 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Draw_pixel(uint8_t x, uint8_t y) {
/// Функция рисования точки.
/// param\ x - координата по X(от 0 до 127)
/// paran\ y - координата по Y(от 0 до 63)
	if( rotation ){
		x = 127-x;
		y = 63-y;
	}
	if (y < SCREEN_HEIGHT && x < SCREEN_WIDTH) {
		ST7920_buffer[(x) + ((y / 8) * 128)] |= 0x01 << y % 8;
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  Функция удаления пикселя на экране
	* @param	( параметры ):  координаты X(от 0 до 127) и Y(от 0 до 63)
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Clear_pixel(uint8_t x, uint8_t y) {
/// Функция удаления точки.
/// param\ x - координата по X(от 0 до 127)
/// paran\ y - координата по Y(от 0 до 63)
	if( rotation ){
			x = 127-x;
			y = 63-y;
	}
	if (y < SCREEN_HEIGHT && x < SCREEN_WIDTH) {
		ST7920_buffer[(x) + ((y / 8) * 128)] &= 0xFE << y % 8;
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  Функция ротации  дисплея ( только в графическом дисплее ).
	* @param	( параметры ):  координаты X(от 0 до 127) и Y(от 0 до 63)
	* @return  ( возвращает ):	0- поворот на 0 градусов ( по умолчанию ), 1- поворот на 180 градусов
								вызвать перед формированием изображения ( если данные в массиве уже есть
								то инверсии не будет, будет только то что было сформировано после вызова данной функции )
	******************************************************************************
*/
void ST7920_rotation(uint8_t mode) {
/// Функция ротации  дисплея ( только в графическом дисплее ).
// вызвать перед формированием изображения ( если данные в массиве уже есть
// то инверсии не будет, будет только то что было сформировано после вызова данной функции )

	if( mode ){
		rotation = 1;
	}
	else{
		rotation = 0;
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция вывода буфера кадра на дисплей
	* @param	( параметры ): Вызывать каждый раз чтобы вывести на экран содержание буфера
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_Update(void) {
	/// Функция вывода изображения на дисплей
	/// Работает с памятью ST7920.
	/// \param *bitmap - изображение 128*64. т.е. Буфер из 1024 элементов.
		uint8_t x, y;
		uint16_t i = 0;
		uint8_t Temp, Db;

		for (y = 0; y < 64; y++) {
			for (x = 0; x < 8; x++) {
				if (y < 32) {
					ST7920_w_cmd(0x80 | y);				//y(0-31)
					ST7920_w_cmd(0x80 | x);				//x(0-8)
				} else {
					ST7920_w_cmd(0x80 | (y - 32));		//y(0-31)
					ST7920_w_cmd(0x88 | x);				//x(0-8)
				}

				i = ((y / 8) * 128) + (x * 16);
				Db = y % 8;

				Temp = (((ST7920_buffer[i] >> Db) & 0x01) << 7) | (((ST7920_buffer[i + 1] >> Db) & 0x01) << 6) | (((ST7920_buffer[i + 2] >> Db) & 0x01) << 5)
						| (((ST7920_buffer[i + 3] >> Db) & 0x01) << 4) | (((ST7920_buffer[i + 4] >> Db) & 0x01) << 3) | (((ST7920_buffer[i + 5] >> Db) & 0x01) << 2)
						| (((ST7920_buffer[i + 6] >> Db) & 0x01) << 1) | (((ST7920_buffer[i + 7] >> Db) & 0x01) << 0);
				ST7920_w_dat(Temp);

				Temp = (((ST7920_buffer[i + 8] >> Db) & 0x01) << 7) | (((ST7920_buffer[i + 9] >> Db) & 0x01) << 6) | (((ST7920_buffer[i + 10] >> Db) & 0x01) << 5)
						| (((ST7920_buffer[i + 11] >> Db) & 0x01) << 4) | (((ST7920_buffer[i + 12] >> Db) & 0x01) << 3) | (((ST7920_buffer[i + 13] >> Db) & 0x01) << 2)
						| (((ST7920_buffer[i + 14] >> Db) & 0x01) << 1) | (((ST7920_buffer[i + 15] >> Db) & 0x01) << 0);

				ST7920_w_dat(Temp);
			}
		}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция инверсии любого места в буфере
	* @param	( параметры ): начальная точка по х от 0 до 1024, конечная точка по y от 0 до 1024
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Inversion(uint16_t x_start, uint16_t x_end) {
/// Функция инверсии любого места в буфере
/// \param x_start - начальная точка по х от 0 до 1024
/// \param x_end - конечная точка по y от 0 до 1024
	for (; x_start < x_end; x_start++) {
		ST7920_buffer[x_start] = ~(ST7920_buffer[x_start]);
	}
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ): Функция очистки буфера кадра ( само изображение на экране не удаляет )
	* @param	( параметры ):
	* @return  ( возвращает ):	

	******************************************************************************
*/
void ST7920_Clear_frame_buffer(void) {
/// Функция очистки буфера кадра
	memset(ST7920_buffer, 0x00, sizeof(ST7920_buffer));
}
//--------------------------------------------------------------------------------

/*
	******************************************************************************
	* @brief	 ( описание ):  функция выводит 1 символ
	* @param	( параметры ):	координаты X Y, символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):

	******************************************************************************
*/
void ST7920_DrawChar(int16_t x, int16_t y, unsigned char ch, FontDef_t* Font, uint8_t multiplier, uint8_t color) {

	uint16_t i, j;

	uint16_t b;

	int16_t X = x, Y = y;

	int16_t xx, yy;

	if( multiplier < 1 ){
		multiplier = 1;
	}

	/* Check available space in LCD */
	if ( SCREEN_WIDTH >= ( x + Font->FontWidth) || SCREEN_HEIGHT >= ( y + Font->FontHeight)){

		/* Go through font */
		for (i = 0; i < Font->FontHeight; i++) {

			if( ch < 127 ){
				b = Font->data[(ch - 32) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch > 191 ){
				// +96 это так как латинские символы и знаки в шрифтах занимают 96 позиций
				// и если в шрифте который содержит сперва латиницу и спец символы и потом
				// только кирилицу то нужно добавлять 95 если шрифт
				// содержит только кирилицу то +96 не нужно
				b = Font->data[((ch - 192) + 96) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 168 ){	// 168 символ по ASCII - Ё
				// 160 эллемент ( символ Ё )
				b = Font->data[( 160 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 184 ){	// 184 символ по ASCII - ё
				// 161 эллемент  ( символ ё )
				b = Font->data[( 161 ) * Font->FontHeight + i];
			}
			//----  Украинская раскладка ----------------------------------------------------
			else if( (uint8_t) ch == 170 ){	// 168 символ по ASCII - Є
				// 162 эллемент ( символ Є )
				b = Font->data[( 162 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 175 ){	// 184 символ по ASCII - Ї
				// 163 эллемент  ( символ Ї )
				b = Font->data[( 163 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 178 ){	// 168 символ по ASCII - І
				// 164 эллемент ( символ І )
				b = Font->data[( 164 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 179 ){	// 184 символ по ASCII - і
				// 165 эллемент  ( символ і )
				b = Font->data[( 165 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 186 ){	// 184 символ по ASCII - є
				// 166 эллемент  ( символ є )
				b = Font->data[( 166 ) * Font->FontHeight + i];
			}
			else if( (uint8_t) ch == 191 ){	// 168 символ по ASCII - ї
				// 167 эллемент ( символ ї )
				b = Font->data[( 167 ) * Font->FontHeight + i];
			}
			//-----------------------------------------------------------------------------

			//-------------------------------------------------------------------------------

			for (j = 0; j < Font->FontWidth; j++) {

				if ((b << j) & 0x8000) {

					for (yy = 0; yy < multiplier; yy++){
						for (xx = 0; xx < multiplier; xx++){
							if(color){
								ST7920_Draw_pixel(X+xx, Y+yy);
							}
							else{
								ST7920_Clear_pixel(X+xx, Y+yy);
							}
						}
					}
				}
				// если фон очищать то оставляем если чтоб фон оставался старый то коментируем эту часть --------------------------------------------
				//-----------------------------------------------------------------------------------------------------------------------------------
				else{

					for (yy = 0; yy < multiplier; yy++){
						for (xx = 0; xx < multiplier; xx++){
							if(!color){
								ST7920_Draw_pixel(X+xx, Y+yy);
							}
							else{
								ST7920_Clear_pixel(X+xx, Y+yy);
							}
						}
					}
				}
				//-----------------------------------------------------------------------------------------------------------------------------------
				//-----------------------------------------------------------------------------------------------------------------------------------

				X = X + multiplier;
			}
			X = x;
			Y = Y + multiplier;
		}
	}
}
//----------------------------------------------------------------------------------


/*
	******************************************************************************
	* @brief	 ( описание ):  функция пишет строку
	* @param	( параметры ):	координаты X Y, символ, шрифт, множитель, цвет 1 или 0
	* @return  ( возвращает ):
	******************************************************************************
*/
void ST7920_Print(int16_t x, int16_t y, char* str, FontDef_t* Font, uint8_t multiplier, uint8_t color) {

	if( multiplier < 1 ){
		multiplier = 1;
	}

	unsigned char buff_char;

	uint16_t len = strlen(str);

	while (len--) {

		//---------------------------------------------------------------------
		// проверка на кириллицу UTF-8, если латиница то пропускаем if
		// Расширенные символы ASCII Win-1251 кириллица (код символа 128-255)
		// проверяем первый байт из двух ( так как UTF-8 ето два байта )
		// если он больше либо равен 0xC0 ( первый байт в кириллеце будет равен 0xD0 либо 0xD1 именно в алфавите )
		if ( (uint8_t)*str >= 0xC0 ){	// код 0xC0 соответствует символу кириллица 'A' по ASCII Win-1251

			// проверяем какой именно байт первый 0xD0 либо 0xD1
			switch ((uint8_t)*str) {
				case 0xD0: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x90 && (uint8_t)*str <= 0xBF){ buff_char = (*str) + 0x30; }	// байт символов А...Я а...п  делаем здвиг на +48
					else if ((uint8_t)*str == 0x81) { buff_char = 0xA8; break; }		// байт символа Ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x84) { buff_char = 0xAA; break; }		// байт символа Є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x86) { buff_char = 0xB2; break; }		// байт символа І ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x87) { buff_char = 0xAF; break; }		// байт символа Ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
				case 0xD1: {
					// увеличиваем массив так как нам нужен второй байт
					str++;
					// проверяем второй байт там сам символ
					if ((uint8_t)*str >= 0x80 && (uint8_t)*str <= 0x8F){ buff_char = (*str) + 0x70; }	// байт символов п...я	елаем здвиг на +112
					else if ((uint8_t)*str == 0x91) { buff_char = 0xB8; break; }		// байт символа ё ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x94) { buff_char = 0xBA; break; }		// байт символа є ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x96) { buff_char = 0xB3; break; }		// байт символа і ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					else if ((uint8_t)*str == 0x97) { buff_char = 0xBF; break; }		// байт символа ї ( если нужнф еще символы добавляем тут и в функции DrawChar() )
					break;
				}
			}
			// уменьшаем еще переменную так как израсходывали 2 байта для кириллицы
			len--;

			ST7920_DrawChar( x, y, buff_char, Font, multiplier, color);
		}
		//---------------------------------------------------------------------
		else{
			ST7920_DrawChar( x, y, *str, Font, multiplier, color);
		}

		x = x + (Font->FontWidth * multiplier);
		/* Increase string pointer */
		str++;
	}
}
//----------------------------------------------------------------------------------

/********************************РАБОТА С ГЕОМЕТРИЧЕСКИМИ ФИГУРАМИ**********************************/

void ST7920_Draw_line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t color) {
	int dx = (x_end >= x_start) ? x_end - x_start : x_start - x_end;
	int dy = (y_end >= y_start) ? y_end - y_start : y_start - y_end;
	int sx = (x_start < x_end) ? 1 : -1;
	int sy = (y_start < y_end) ? 1 : -1;
	int err = dx - dy;

	for (;;) {
		if(color){
			ST7920_Draw_pixel(x_start, y_start);
		}
		else{
			ST7920_Clear_pixel(x_start, y_start);
		}
		
		if (x_start == x_end && y_start == y_end)
			break;
		int e2 = err + err;
		if (e2 > -dy) {
			err -= dy;
			x_start += sx;
		}
		if (e2 < dx) {
			err += dx;
			y_start += sy;
		}
	}
}


/*--------------------------------Вывести пустотелый прямоугольник---------------------------------*/
void ST7920_Draw_rectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
/// Вывести пустотелый прямоугольник
/// \param x - начальная точка по оси "x"
/// \param y - начальная точка по оси "y"
/// \param width - ширина прямоугольника
/// \param height - высота прямоугольника

	/*Проверка ширины и высоты*/
	if ((x + width) >= SCREEN_WIDTH) {
		width = SCREEN_WIDTH - x;
	}
	if ((y + height) >= SCREEN_HEIGHT) {
		height = SCREEN_HEIGHT - y;
	}

	/*Рисуем линии*/
	ST7920_Draw_line(x, y, x + width, y, color); /*Верх прямоугольника*/
	ST7920_Draw_line(x, y + height, x + width, y + height, color); /*Низ прямоугольника*/
	ST7920_Draw_line(x, y, x, y + height, color); /*Левая сторона прямоугольника*/
	ST7920_Draw_line(x + width, y, x + width, y + height, color); /*Правая сторона прямоугольника*/
}
/*--------------------------------Вывести пустотелый прямоугольник---------------------------------*/

/*-------------------------------Вывести закрашенный прямоугольник---------------------------------*/
void ST7920_Draw_rectangle_filled(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t color) {
/// Вывести закрашенный прямоугольник
/// \param x - начальная точка по оси "x"
/// \param y - начальная точка по оси "y"
/// \param width - ширина прямоугольника
/// \param height - высота прямоугольника

	/*Проверка ширины и высоты*/
	if ((x + width) >= SCREEN_WIDTH) {
		width = SCREEN_WIDTH - x;
	}
	if ((y + height) >= SCREEN_HEIGHT) {
		height = SCREEN_HEIGHT - y;
	}

	/*Рисуем линии*/
	for (uint8_t i = 0; i <= height; i++) {
		ST7920_Draw_line(x, y + i, x + width, y + i, color);
	}
}
/*-------------------------------Вывести закрашенный прямоугольник---------------------------------*/

/*---------------------------------Вывести пустотелую окружность-----------------------------------*/
void ST7920_Draw_circle(uint8_t x, uint8_t y, uint8_t radius, uint8_t color) {
/// Вывести пустотелую окружность
/// \param x - точка центра окружности по оси "x"
/// \param y - точка центра окружности по оси "y"
/// \param radius - радиус окружности

	int f = 1 - (int) radius;
	int ddF_x = 1;

	int ddF_y = -2 * (int) radius;
	int x_0 = 0;
	
	if(color){
		ST7920_Draw_pixel(x, y + radius);
		ST7920_Draw_pixel(x, y - radius);
		ST7920_Draw_pixel(x + radius, y);
		ST7920_Draw_pixel(x - radius, y);
	}
	else{
		ST7920_Clear_pixel(x, y + radius);
		ST7920_Clear_pixel(x, y - radius);
		ST7920_Clear_pixel(x + radius, y);
		ST7920_Clear_pixel(x - radius, y);
	}
	

	int y_0 = radius;
	while (x_0 < y_0) {
		if (f >= 0) {
			y_0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x_0++;
		ddF_x += 2;
		f += ddF_x;
		if(color){
			ST7920_Draw_pixel(x + x_0, y + y_0);
			ST7920_Draw_pixel(x - x_0, y + y_0);
			ST7920_Draw_pixel(x + x_0, y - y_0);
			ST7920_Draw_pixel(x - x_0, y - y_0);
			ST7920_Draw_pixel(x + y_0, y + x_0);
			ST7920_Draw_pixel(x - y_0, y + x_0);
			ST7920_Draw_pixel(x + y_0, y - x_0);
			ST7920_Draw_pixel(x - y_0, y - x_0);
		}
		else{
			ST7920_Clear_pixel(x + x_0, y + y_0);
			ST7920_Clear_pixel(x - x_0, y + y_0);
			ST7920_Clear_pixel(x + x_0, y - y_0);
			ST7920_Clear_pixel(x - x_0, y - y_0);
			ST7920_Clear_pixel(x + y_0, y + x_0);
			ST7920_Clear_pixel(x - y_0, y + x_0);
			ST7920_Clear_pixel(x + y_0, y - x_0);
			ST7920_Clear_pixel(x - y_0, y - x_0);
		}
	}
}
/*---------------------------------Вывести пустотелую окружность-----------------------------------*/

/*--------------------------------Вывести закрашенную окружность-----------------------------------*/
void ST7920_Draw_circle_filled(int16_t x, int16_t y, int16_t radius, uint8_t color) {
/// Вывести закрашенную окружность
/// \param x - точка центра окружности по оси "x"
/// \param y - точка центра окружности по оси "y"
/// \param radius - радиус окружности

	int16_t f = 1 - radius;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * radius;
	int16_t x_0 = 0;
	int16_t y_0 = radius;

	if(color){
		ST7920_Draw_pixel(x, y + radius);
		ST7920_Draw_pixel(x, y - radius);
		ST7920_Draw_pixel(x + radius, y);
		ST7920_Draw_pixel(x - radius, y);
	}
	else{
		ST7920_Clear_pixel(x, y + radius);
		ST7920_Clear_pixel(x, y - radius);
		ST7920_Clear_pixel(x + radius, y);
		ST7920_Clear_pixel(x - radius, y);
	}
	
	
	
	ST7920_Draw_line(x - radius, y, x + radius, y, color);

	while (x_0 < y_0) {
		if (f >= 0) {
			y_0--;
			ddF_y += 2;
			f += ddF_y;
		}
		x_0++;
		ddF_x += 2;
		f += ddF_x;

		ST7920_Draw_line(x - x_0, y + y_0, x + x_0, y + y_0, color);
		ST7920_Draw_line(x + x_0, y - y_0, x - x_0, y - y_0, color);
		ST7920_Draw_line(x + y_0, y + x_0, x - y_0, y + x_0, color);
		ST7920_Draw_line(x + y_0, y - x_0, x - y_0, y - x_0, color);
	}
}
/*--------------------------------Вывести закрашенную окружность-----------------------------------*/

/*-----------------------------------Вывести пустотелый треугольник--------------------------------*/
void ST7920_Draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color) {
/// Вывести пустотелый треугольник
/// \param x_1 - первая точка треугольника. Координата по оси "x"
/// \param y_1 - первая точка треугольника. Координата по оси "y"
/// \param x_2 - вторая точка треугольника. Координата по оси "x"
/// \param y_2 - вторая точка треугольника. Координата по оси "y"
/// \param x_3 - третья точка треугольника. Координата по оси "x"
/// \param y_3 - третья точка треугольника. Координата по оси "y"

	ST7920_Draw_line(x1, y1, x2, y2, color);
	ST7920_Draw_line(x2, y2, x3, y3, color);
	ST7920_Draw_line(x3, y3, x1, y1, color);
}
/*-----------------------------------Вывести пустотелый треугольник--------------------------------*/

/*----------------------------------Вывести закрашенный треугольник--------------------------------*/
void ST7920_Draw_triangle_filled(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint8_t color) {
/// Вывести закрашенный треугольник
/// \param x_1 - первая точка треугольника. Координата по оси "x"
/// \param y_1 - первая точка треугольника. Координата по оси "y"
/// \param x_2 - вторая точка треугольника. Координата по оси "x"
/// \param y_2 - вторая точка треугольника. Координата по оси "y"
/// \param x_3 - третья точка треугольника. Координата по оси "x"
/// \param y_3 - третья точка треугольника. Координата по оси "y"

int16_t deltax = 0;
int16_t deltay = 0;
int16_t x = 0;
int16_t y = 0;
int16_t xinc1 = 0;
int16_t xinc2 = 0;
int16_t yinc1 = 0;
int16_t yinc2 = 0;
int16_t den = 0;
int16_t num = 0;
int16_t numadd = 0;
int16_t numpixels = 0;
int16_t curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7920_Draw_line(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}
/*----------------------------------Вывести закрашенный треугольник--------------------------------*/


//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( заполненый )
//==============================================================================
void ST7920_DrawFillRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint8_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7920_DrawRectangleFilled(x + cornerRadius, y, x + cornerRadius + width - 2 * cornerRadius, y + height, color);
  // draw four corners
  ST7920_DrawFillCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 1, height - 2 * cornerRadius - 1, color);
  ST7920_DrawFillCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 2, height - 2 * cornerRadius - 1, color);
}
//==============================================================================

//==============================================================================
// Процедура рисования половины окружности ( правая или левая ) ( заполненый )
//==============================================================================
void ST7920_DrawFillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint8_t color) {

  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;
  int16_t px = x;
  int16_t py = y;

  delta++; // Avoid some +1's in the loop

  while (x < y) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    if (x < (y + 1)) {
      if (corners & 1){
        ST7920_DrawLine(x0 + x, y0 - y, x0 + x, y0 - y - 1 + 2 * y + delta, color);
			}
      if (corners & 2){
        ST7920_DrawLine(x0 - x, y0 - y, x0 - x, y0 - y - 1 + 2 * y + delta, color);
			}
    }
    if (y != py) {
      if (corners & 1){
        ST7920_DrawLine(x0 + py, y0 - px, x0 + py, y0 - px - 1 + 2 * px + delta, color);
			}
      if (corners & 2){
        ST7920_DrawLine(x0 - py, y0 - px, x0 - py, y0 - px - 1 + 2 * px + delta, color);
			}
			py = y;
    }
    px = x;
  }
}
//==============================================================================																		

//==============================================================================
// Процедура рисования четверти окружности (закругление, дуга) ( ширина 1 пиксель)
//==============================================================================
void ST7920_DrawCircleHelper(int16_t x0, int16_t y0, int16_t radius, int8_t quadrantMask, uint8_t color)
{
    int16_t f = 1 - radius ;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * radius;
    int16_t x = 0;
    int16_t y = radius;

    while (x <= y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
				
        x++;
        ddF_x += 2;
        f += ddF_x;
		if(color){
			if (quadrantMask & 0x4) {
				ST7920_DrawPixel(x0 + x, y0 + y, color);
				ST7920_DrawPixel(x0 + y, y0 + x, color);
			}
			if (quadrantMask & 0x2) {
				ST7920_DrawPixel(x0 + x, y0 - y, color);
				ST7920_DrawPixel(x0 + y, y0 - x, color);
			}
			if (quadrantMask & 0x8) {
				ST7920_DrawPixel(x0 - y, y0 + x, color);
				ST7920_DrawPixel(x0 - x, y0 + y, color);
			}
			if (quadrantMask & 0x1) {
				ST7920_DrawPixel(x0 - y, y0 - x, color);
				ST7920_DrawPixel(x0 - x, y0 - y, color);
			}
		}
		else{
			if (quadrantMask & 0x4) {
				ST7920_Clear_pixel(x0 + x, y0 + y, color);
				ST7920_Clear_pixel(x0 + y, y0 + x, color);
			}
			if (quadrantMask & 0x2) {
				ST7920_Clear_pixel(x0 + x, y0 - y, color);
				ST7920_Clear_pixel(x0 + y, y0 - x, color);
			}
			if (quadrantMask & 0x8) {
				ST7920_Clear_pixel(x0 - y, y0 + x, color);
				ST7920_Clear_pixel(x0 - x, y0 + y, color);
			}
			if (quadrantMask & 0x1) {
				ST7920_Clear_pixel(x0 - y, y0 - x, color);
				ST7920_Clear_pixel(x0 - x, y0 - y, color);
			}
		}
        
    }
}
//==============================================================================		

//==============================================================================
// Процедура рисования прямоугольник с закругленніми краями ( пустотелый )
//==============================================================================
void ST7920_DrawRoundRect(int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t cornerRadius, uint8_t color) {
	
	int16_t max_radius = ((width < height) ? width : height) / 2; // 1/2 minor axis
  if (cornerRadius > max_radius){
    cornerRadius = max_radius;
	}
	
  ST7920_DrawLine(x + cornerRadius, y, x + cornerRadius + width -1 - 2 * cornerRadius, y, color);         // Top
  ST7920_DrawLine(x + cornerRadius, y + height - 1, x + cornerRadius + width - 1 - 2 * cornerRadius, y + height - 1, color); // Bottom
  ST7920_DrawLine(x, y + cornerRadius, x, y + cornerRadius + height - 1 - 2 * cornerRadius, color, color);         // Left
  ST7920_DrawLine(x + width - 1, y + cornerRadius, x + width - 1, y + cornerRadius + height - 1 - 2 * cornerRadius, color); // Right
	
  // draw four corners
	ST7920_DrawCircleHelper(x + cornerRadius, y + cornerRadius, cornerRadius, 1, color);
  ST7920_DrawCircleHelper(x + width - cornerRadius - 1, y + cornerRadius, cornerRadius, 2, color);
	ST7920_DrawCircleHelper(x + width - cornerRadius - 1, y + height - cornerRadius - 1, cornerRadius, 4, color);
  ST7920_DrawCircleHelper(x + cornerRadius, y + height - cornerRadius - 1, cornerRadius, 8, color);
}
//==============================================================================

//==============================================================================
// Процедура рисования линия толстая ( последний параметр толщина )
//==============================================================================
void ST7920_DrawLineThick(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint8_t color, uint8_t thick) {
	const int16_t deltaX = abs(x2 - x1);
	const int16_t deltaY = abs(y2 - y1);
	const int16_t signX = x1 < x2 ? 1 : -1;
	const int16_t signY = y1 < y2 ? 1 : -1;

	int16_t error = deltaX - deltaY;

	if (thick > 1){
		ST7920_DrawCircleFilled(x2, y2, thick >> 1, color);
	}
	else{
		if(color){
			ST7920_DrawPixel(x2, y2, color);
		}
		else{
			ST7920_Clear_pixel(x2, y2, color);
		}
	}

	while (x1 != x2 || y1 != y2) {
		if (thick > 1){
			ST7920_DrawCircleFilled(x1, y1, thick >> 1, color);
		}
		else{
			if(color){
				ST7920_DrawPixel(x1, y1, color);
			}
			else{
				ST7920_Clear_pixel(x1, y1, color);
			}
		}

		const int16_t error2 = error * 2;
		if (error2 > -deltaY) {
			error -= deltaY;
			x1 += signX;
		}
		if (error2 < deltaX) {
			error += deltaX;
			y1 += signY;
		}
	}
}
//==============================================================================		

//==============================================================================
// Процедура рисования дуга толстая ( часть круга )
//==============================================================================
void ST7920_DrawArc(int16_t x0, int16_t y0, int16_t radius, int16_t startAngle, int16_t endAngle, uint8_t color, uint8_t thick) {
	
	int16_t xLast = -1, yLast = -1;
	startAngle -= 90;
	endAngle -= 90;

	for (int16_t angle = startAngle; angle <= endAngle; angle += 2) {
		float angleRad = (float) angle * PI / 180;
		int x = cos(angleRad) * radius + x0;
		int y = sin(angleRad) * radius + y0;

		if (xLast == -1 || yLast == -1) {
			xLast = x;
			yLast = y;
			continue;
		}

		if (thick > 1){
			ST7920_DrawLineThick(xLast, yLast, x, y, color, thick);
		}
		else{
			ST7920_DrawLine(xLast, yLast, x, y, color);
		}

		xLast = x;
		yLast = y;
	}
}
//==============================================================================


/********************************РАБОТА С ГЕОМЕТРИЧЕСКИМИ ФИГУРАМИ**********************************/




/************************ (C) COPYRIGHT GKP *****END OF FILE****/
