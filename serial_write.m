clear 
clc
s1 = serial('COM5','BaudRate',9600,'Tag', 'Arduino');
fopen(s1);
fwrite(s1,'AAAAAAAAAAAAAA');
fclose(s1);
clear s1;

