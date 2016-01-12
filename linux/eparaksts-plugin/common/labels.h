/*
 * Estonian ID card plugin for web browsers
 *
 * Copyright (C) 2010-2011 Codeborne <info@codeborne.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ESTEID_LABELS_H
#define	ESTEID_LABELS_H


label labels[] = {
	// Labels format: {"English label", "Estonian translation", "Russian translation"},
	// NB! English label should not be changed here unless the place where it is used is also changed
	// All text should be in UTF8 encoding
	
	// Certificate selection dialog:
	{"Select certificate", "Sertifikāta izvēle", "Выбор сертификата"},
	{"Certificate", "Sertifikāts", "Сертификат"},
	{"Type", "Tips", "Тип"},
	{"Valid to", "Derīgs līdz", "Действительный до"},
	{"Select", "Izvēlēties",  "Выбрать"},
	{"Cancel", "Atcelt", "Отменить"},
	{"Details...", "Detaļas...", "Детали..."},
	{"Sign", "Parakstīt", "Подписать"},
	{"By selecting a certificate I accept that my name and personal ID code will be sent to service provider.", "Izvēloties sertifikātu es apstiprinu, ka mans vārds, uzvārds un personas kods tiks nosūtīts pakalpojuma sniedzējam.", "Выбирая сертификат, я соглащаюсь с тем, что мое имя и личный код будут переданы представителю услуг."},

	// PIN2 dialog and PIN pad message box:
	{"For signing enter PIN2:", "Lai parakstītu ievadiet PIN2:", "Для подписания введите PIN2:" },
	{"Tries left:", "Atlikuši mēģinājumi:", "Возможных попыток:" },
	{"Incorrect PIN2! ", "Nepareizs PIN2! ", "Неправильный PIN2! "},
	{"PIN2 blocked, cannot sign!", "PIN2 bloķēts, parakstīt nav iespējams!", "PIN2 блокирован, невозможно подписать!"},
	{"Signing", "Parakstīšana", "Подписание"},
	{"Error", "Kļūda", "Ошибка"},	
	{"For signing enter PIN2 from PIN pad", "Lai parakstītu ievadiet PIN2 no PIN ievades ierīces", "Для подписания введите PIN2 с PIN-клавиатуры"}
};

#endif

