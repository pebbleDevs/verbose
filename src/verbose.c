// Filename: vebose.c
// Version: 1.0.0 
//                                                         
// Purpose:   Pebble Verbose Watchface                      
//                                                         
// Author:   Brian K. Holman (me@brianholman.com)      
//                                               

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xA4, 0x1B, 0xB0, 0xE2, 0xD2, 0x62, 0x4E, 0xA6, 0xAA, 0x30, 0xED, 0xBE, 0x01, 0xE3, 0x8A, 0x02}
PBL_APP_INFO(MY_UUID, "Verbose", "Brian Holman", 1, 0 /* App version */, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

TextLayer text_dow_layer;
TextLayer text_day_layer;
TextLayer text_month_layer;
TextLayer text_year_layer;
TextLayer text_hrmintime_layer;
TextLayer text_am_pm_layer;

void update_display(PblTm *tick_time) 
{

	static char dow_text[] = "Wednesday,";
	static char day_text[] = "25th of";
	static char month_text[] = "September";
	static char year_text[] = "1970";
	static char hrmintime_text[] = "11:59";
	static char am_pm_text[] = "PM ";
	static int tm_mday = -1;
	static int tm_mon = -1;
	static int tm_year = -1;
	static int tm_wday = -1;

	char *time_format;	

	// Only update weekday text if weekday has changed from last display
	if (tm_wday != tick_time->tm_wday)
	{
		string_format_time(dow_text, sizeof(dow_text), "%A,", tick_time);
		text_layer_set_text(&text_dow_layer, dow_text);
		tm_wday = tick_time->tm_wday;
	}

	// Only update day of the month text if the day of the month has changed from last display
	if (tm_mday != tick_time->tm_mday)
	{
		string_format_time(day_text, sizeof(day_text), "%eth of", tick_time);
		text_layer_set_text(&text_day_layer, day_text);
	
		// Handle the special cases of ordinalization of the day of the month
		if (!(tick_time->tm_mday >=11 && tick_time->tm_mday <= 13))
		{
			switch (tick_time->tm_mday%10)
			{
				case 1:
					day_text[2] = 's';
					day_text[3] = 't';			
					break;
				case 2:
					day_text[2] = 'n';
					day_text[3] = 'd'; 
					break;
				case 3:
					day_text[2] = 'r';
					day_text[3] = 'd'; 			
					break;
			}
		}
		
		tm_mday = tick_time->tm_mday;
	}

	// Only update the month name text if the month has changed
	if (tm_mon != tick_time->tm_mon)
	{
		string_format_time(month_text, sizeof(month_text), "%B", tick_time);
		text_layer_set_text(&text_month_layer, month_text);
		tm_mon = tick_time->tm_mon;
	}

	// Only update the year text if the year has changed
	if (tm_year != tick_time->tm_year)
	{
		string_format_time(year_text, sizeof(year_text), "%Y", tick_time);
		text_layer_set_text(&text_year_layer, year_text);
		tm_year = tick_time->tm_year;
	}

	if (clock_is_24h_style()) 
	{
		time_format = "%R";
	} 
	else 
	{
		time_format = "%I:%M";
	}

	string_format_time(hrmintime_text, sizeof(hrmintime_text), time_format, tick_time);
	if (!clock_is_24h_style() && (hrmintime_text[0] == '0'))
	{
		memmove(hrmintime_text, &hrmintime_text[1], sizeof(hrmintime_text) - 1);	
	}
	text_layer_set_text(&text_hrmintime_layer, hrmintime_text);

	string_format_time(am_pm_text, sizeof(am_pm_text), "%p ", tick_time);
	if (!clock_is_24h_style())
	{
		text_layer_set_text(&text_am_pm_layer, am_pm_text);		
	}
	else
	{
		text_layer_set_text(&text_am_pm_layer, "24h");
	}
}

void handle_init(AppContextRef ctx) 
{
	(void)ctx;

	window_init(&window, "Verbose");
	window_stack_push(&window, true /* Animated */);
	window_set_background_color(&window, GColorBlack);

	resource_init_current_app(&APP_RESOURCES);

	text_layer_init(&text_dow_layer, window.layer.frame);
	text_layer_set_text_color(&text_dow_layer, GColorWhite);
	text_layer_set_background_color(&text_dow_layer, GColorClear);
	text_layer_set_text_alignment(&text_dow_layer, GTextAlignmentCenter);
	layer_set_frame(&text_dow_layer.layer, GRect(0, 0, 144, 168));
	text_layer_set_font(&text_dow_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_21)));
	layer_add_child(&window.layer, &text_dow_layer.layer);

	text_layer_init(&text_day_layer, window.layer.frame);
	text_layer_set_text_color(&text_day_layer, GColorWhite);
	text_layer_set_background_color(&text_day_layer, GColorClear);
	text_layer_set_text_alignment(&text_day_layer, GTextAlignmentCenter);
	layer_set_frame(&text_day_layer.layer, GRect(0, 24, 144, 168-24));
	text_layer_set_font(&text_day_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_21)));
	layer_add_child(&window.layer, &text_day_layer.layer);

	text_layer_init(&text_month_layer, window.layer.frame);
	text_layer_set_text_color(&text_month_layer, GColorWhite);
	text_layer_set_text_alignment(&text_month_layer, GTextAlignmentCenter);
	text_layer_set_background_color(&text_month_layer, GColorClear);
	layer_set_frame(&text_month_layer.layer, GRect(0, 48, 144, 168-48));
	text_layer_set_font(&text_month_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_21)));
	layer_add_child(&window.layer, &text_month_layer.layer);

	text_layer_init(&text_year_layer, window.layer.frame);
	text_layer_set_text_color(&text_year_layer, GColorWhite);
	text_layer_set_text_alignment(&text_year_layer, GTextAlignmentCenter);
	text_layer_set_background_color(&text_year_layer, GColorClear);
	layer_set_frame(&text_year_layer.layer, GRect(0, 72, 144, 168-72));
	text_layer_set_font(&text_year_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_21)));
	layer_add_child(&window.layer, &text_year_layer.layer);

	text_layer_init(&text_hrmintime_layer, window.layer.frame);
	text_layer_set_text_color(&text_hrmintime_layer, GColorWhite);
	text_layer_set_text_alignment(&text_hrmintime_layer, GTextAlignmentCenter);
	text_layer_set_background_color(&text_hrmintime_layer, GColorClear);
	layer_set_frame(&text_hrmintime_layer.layer, GRect(0, 96, 144, 168-96));
	text_layer_set_font(&text_hrmintime_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_48)));
	layer_add_child(&window.layer, &text_hrmintime_layer.layer);

	text_layer_init(&text_am_pm_layer, window.layer.frame);
	text_layer_set_text_color(&text_am_pm_layer, GColorWhite);
	text_layer_set_text_alignment(&text_am_pm_layer, GTextAlignmentRight);
	text_layer_set_background_color(&text_am_pm_layer, GColorClear);
	layer_set_frame(&text_am_pm_layer.layer, GRect(0, 147, 144, 168-147));
	text_layer_set_font(&text_am_pm_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_COOPERBLK_SUBSET_21)));
	layer_add_child(&window.layer, &text_am_pm_layer.layer);

	PblTm tick_time;
	get_time(&tick_time);
	update_display(&tick_time);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) 
{

	(void)ctx;
	update_display(t->tick_time);

}

void pbl_main(void *params) 
{
	PebbleAppHandlers handlers = 
	{
		.init_handler = &handle_init,

			.tick_info = 
		{
			.tick_handler = &handle_minute_tick,
				.tick_units = MINUTE_UNIT
		}
	};
	app_event_loop(params, &handlers);
}
