* PERSONAL DIARY *!file 1 diary!scroll 5,12!temp scrline,,i1!temp scrsave,,i1!temp maxline,,i1!temp date,Today is,d4!temp seldate,,d4!temp selday,Diary for,a9!temp deltime, ,m4!temp day,,a9(7)!temp ttime,Time,m4(12)!temp text, ,a54(12)+selday,,3,17+seldate, ,3,29+deltime,,3,40,##.##+date,,3,63+ttime,,5,9,##.##+text,,5,17    scroll 1: day="Sunday"    scroll 2: day="Monday"    scroll 3: day="Tuesday"    scroll 4: day="Wednesday"    scroll 5: day="Thursday"    scroll 6: day="Friday"    scroll 7: day="Saturday"    maxline=12: display date    end*f=find day    gosub INPUT_DATEF1  display selday-seldate,date    find 1 key=seldate nsr=F4    scroll 1F2  ttime=dy_time: display ttimeF3  text=dy_text: display text    match 1 nsr=END    scroll: if dy_line>1 then goto F3    if dy_count<=maxline-scrline then scroll: goto F2    prompt "Continue" no=END    gosub CLEAR_SCROLL: message "Continued"    scroll 1: goto F2F4  message "No entries": end*n=next day    if seldate=0 then seldate=date    clear: seldate=seldate+1    scroll seldate%7+1: selday=day    goto F1*i=insert    message "": gosub CLEAR_SCROLL    if seldate=0 then\      gosub INPUT_DATE: display selday-seldate,date    scroll 1I1  input ttime bs=END: dy_time=ttime    if ttime>2359 or ttime%100>59 then error "": goto I1    testkey 1 key=seldate,dy_time,scrline nsr=I2    error "An entry exists for this time": goto I1I2  input text bs=I3    if text="" then goto I4    if scrline<maxline then scroll: goto I2    dy_count=scrline: goto I5I3  if scrline=1 then goto I1    scroll -1: goto I2I4  gosub COMPRESS: if text<>"" then goto I2    dy_count=scrline-1I5  prompt "All correct" no=I2: scroll 1I6  if scrline>dy_count then clear: goto F1    dy_text=text    insert 1 key=seldate,dy_time,scrline    scroll: goto I6*d=delete    if seldate=0 then error "No day selected": end    message "Enter time of entry to be deleted"    input deltime bs=END    find 1 key=seldate,deltime nsr=D8D1  if dy_time<>deltime then goto D2    delete 1    match 1 nsr=D2: goto D1D2  clear: goto F1D8  error "No entry for this time": end*p=purge    clear: display date    message "Enter date to which diary is to be cleared"P1  input seldate bs=P3    if seldate>date then\      prompt "Future date! - Are you sure" no=P1    rewind 1: message "Purge in progress..."P2  next 1 nsr=P3    if dy_date<=seldate then delete 1: goto P2P3  clear: seldate=0: display date: end*e=exit    exitINPUT_DATE\    clear: display date    input seldate bs=END    if seldate=0 then seldate=date    scroll seldate%7+1: selday=day    returnCLEAR_SCROLL\    scroll maxlineCS1 clear ttime-text    if scrline>1 then scroll -1: goto CS1    returnCOMPRESS\    scrsave=scrlineCO1 if scrline=maxline then goto CO2    scroll: if text="" then goto CO2    dy_text=text: clear text    scroll -1: text=dy_text: display text    scroll: goto CO1CO2 scroll scrsave: returnEND end