x = serial('/dev/cu.usbmodem14101', 'BAUD', 9600);

fopen(x);

go = true;

while go
    a = input('Input the number of connected terminals, Press 0 to break: ');
    
    if (a == 0)
        go = false;
    else
        fprintf(x, '%c', a);
    end
end

fclose(x);
