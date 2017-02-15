function instr_callback(obj,event)

global txt;
global dataTab;
global index;

    display('Data Received')
%      A = fread(obj);
    if (obj.BytesAvailable > 0)
        %A = fread(obj, obj.BytesAvailable,'uchar')        
        txt = fgetl(obj)

        str = txt(isstrprop(txt,'alpha'));
        num = txt(isstrprop(txt,'digit'));
        if strcmp(str,'TAmiliSec')
            dataTab(index,3) = str2num(num);            
        end
        if strcmp(str,'TBmiliSec')
            dataTab(index,4) = str2num(num);            
        end        
    end
end

% s1.BytesAvailable