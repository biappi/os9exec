!file diary!temp day,,a9(7),t!temp name,,a30,t!temp from_date,,d4!temp to_date,,d4!input "Please enter your name";name!input "From date";from_date!input "  To date";to_date!title gosub INITIALISE!title print: print "Personal Diary for ";name;!title print " from ";from_date;!title if to_date then print " to ";to_date;!title print: print!select if dy_date>=from_date and (to_date=0 or dy_date<=to_date)!on starting dy_date gosub NEW_DAY!on ending dy_date print: end!on ending dy_time print    if dy_line=1 then gosub NEW_TIME    print tab(8);dy_text    endNEW_DAY\    keep dy_count+2    scroll dy_date%7+1    print day,dy_date    returnNEW_TIME\    keep dy_count+1    print spc(2);dy_time;    returnINITIALISE\    if from_date=0 then from_date=date    scroll 1: day="Sunday"    scroll 2: day="Monday"    scroll 3: day="Tuesday"    scroll 4: day="Wednesday"    scroll 5: day="Thursday"    scroll 6: day="Friday"    scroll 7: day="Saturday"    return