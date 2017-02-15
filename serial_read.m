clear all
clc

global txt;
global dataTab;
global index;

dataTab = zeros(1,4);
index = 1;
%% codes should be executed on GUI open event
s1 = serial('COM5','BaudRate',9600,'Tag', 'Arduino');
s1.Terminator = 'LF';
s1.BytesAvailableFcnCount = 1;
%s1.BytesAvailableFcnMode = 'byte';
s1.BytesAvailableFcnMode =  'terminator';
s1.BytesAvailableFcn = @instr_callback;
fopen(s1);
% A = fread(s1);



%% codes should be executed on GUI close event
% fclose(s1);
% delete(s1);

% ddd = '12hjb42&34ni3&(*&';                   %# Your sample string
% alphaStr = ddd(isstrprop(ddd,'alpha'))      %# Get the alphabetic characters
% digitStr = str(isstrprop(ddd,'digit'))      %# Get the numeric characters
% otherStr = str(~isstrprop(ddd,'alphanum'))

