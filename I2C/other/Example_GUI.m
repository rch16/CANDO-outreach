classdef Example_GUI < matlab.apps.AppBase

    % Properties that correspond to app components
    properties (Access = public)
        AD5933ApplicationUIFigure       matlab.ui.Figure
        TabGroup                        matlab.ui.container.TabGroup
        COMPortSettingsTab              matlab.ui.container.Tab
        OpenButton                      matlab.ui.control.Button
        CloseButton                     matlab.ui.control.Button
        TestConnectionButton            matlab.ui.control.Button
        ConnectionStatusLabel           matlab.ui.control.Label
        COMPortDropDownLabel            matlab.ui.control.Label
        COMPortDropDown                 matlab.ui.control.DropDown
        BaudRateDropDownLabel           matlab.ui.control.Label
        BaudRateDropDown                matlab.ui.control.DropDown
        RefreshButton                   matlab.ui.control.Button
        TestStatusLabel                 matlab.ui.control.Label
        ClearBufferButton               matlab.ui.control.Button
        ImpedanceSpectroscopyTab        matlab.ui.container.Tab
        ControlPanel                    matlab.ui.container.Panel
        StartFrequencyHzEditFieldLabel  matlab.ui.control.Label
        StartFrequencyHzEditField       matlab.ui.control.NumericEditField
        FrequencyIncrementHzEditFieldLabel  matlab.ui.control.Label
        FrequencyIncrementHzEditField   matlab.ui.control.NumericEditField
        NumberofIncrementsEditFieldLabel  matlab.ui.control.Label
        NumberofIncrementsEditField     matlab.ui.control.NumericEditField
        NumberofSettlingCyclesEditFieldLabel  matlab.ui.control.Label
        NumberofSettlingCyclesEditField  matlab.ui.control.NumericEditField
        EndFrequencyHzEditFieldLabel    matlab.ui.control.Label
        EndFrequencyHzEditField         matlab.ui.control.NumericEditField
        MCLKFrequencyHzEditFieldLabel   matlab.ui.control.Label
        MCLKFrequencyHzEditField        matlab.ui.control.NumericEditField
        WritecontrolvaluesButton        matlab.ui.control.Button
        MultiplierButtonGroup           matlab.ui.container.ButtonGroup
        x1Button                        matlab.ui.control.RadioButton
        x2Button                        matlab.ui.control.RadioButton
        x4Button                        matlab.ui.control.RadioButton
        ControlstatusLabel              matlab.ui.control.Label
        TabGroup2                       matlab.ui.container.TabGroup
        MagnitudeTab                    matlab.ui.container.Tab
        MagnitudeAxes                   matlab.ui.control.UIAxes
        PhaseTab                        matlab.ui.container.Tab
        PhaseAxes                       matlab.ui.control.UIAxes
        PerformsweepButton              matlab.ui.control.Button
        SweepstatusLabel                matlab.ui.control.Label
        PGAGainButtonGroup              matlab.ui.container.ButtonGroup
        PGA_x1_button                   matlab.ui.control.RadioButton
        PGA_x5_button                   matlab.ui.control.RadioButton
        OutputRangeButtonGroup          matlab.ui.container.ButtonGroup
        range_1_button                  matlab.ui.control.RadioButton
        range_2_button                  matlab.ui.control.RadioButton
        range_3_button                  matlab.ui.control.RadioButton
        range_4_button                  matlab.ui.control.RadioButton
        SaveButton                      matlab.ui.control.Button
        FilenameEditFieldLabel          matlab.ui.control.Label
        FilenameEditField               matlab.ui.control.EditField
        LocationEditFieldLabel          matlab.ui.control.Label
        LocationEditField               matlab.ui.control.EditField
        BrowseButton                    matlab.ui.control.Button
        xlsxLabel                       matlab.ui.control.Label
        SavestatusLabel                 matlab.ui.control.Label
        CalibrationTab                  matlab.ui.container.Tab
        PerformcalibrationsweepButton   matlab.ui.control.Button
        GainFactorAxes                  matlab.ui.control.UIAxes
        CalibrationstatusLabel          matlab.ui.control.Label
        MeangainfactorLabel             matlab.ui.control.Label
        SystemPhaseAxes                 matlab.ui.control.UIAxes
        MeansystemphaseLabel            matlab.ui.control.Label
        CalibrationresistanceOhmsEditFieldLabel  matlab.ui.control.Label
        CalibrationresistanceOhmsEditField  matlab.ui.control.NumericEditField
        LowGainFactorLabel              matlab.ui.control.Label
        HighGainFactorLabel             matlab.ui.control.Label
        FlowRateTab                     matlab.ui.container.Tab
        VdiffAbsoluteAxes               matlab.ui.control.UIAxes
        VdiffRelativeAxes               matlab.ui.control.UIAxes
        StartrecordingButton            matlab.ui.control.Button
        StoprecordingButton             matlab.ui.control.Button
        AbsoluteLabel                   matlab.ui.control.Label
        RelativeLabel                   matlab.ui.control.Label
    end

    
    properties (Access = private)
        serialPort = serial('COM'); % Description
        AnalogTempTimer % Timer for getting the analog temperature
        VdiffAbsoluteLine % Description
        VdiffRelativeLine % Description
        complexImpedanceData % Description
        magnitudeData % Description
        phaseData
        frequencyArray % Description
        gainFactorArray % Description
        impedanceDataArray % Description
        systemPhaseArray % Description
        highGainFactor % Description
        lowGainFactor % Description
        gainFactorVariation % Description
        linearGainFactorArray % Description
        lowSystemPhase % Description
        highSystemPhase % Description
        systemPhaseVariation % Description
        linearSystemPhaseArray % Description
        saveLocation % Description
    end
    
    methods (Access = private)
        
        
        function results = closePort(app)
            status = app.serialPort.status;
            if(status == 'open')
                fclose(app.serialPort);
            else
                %port already closed
            end
            status = app.serialPort.status;
            app.ConnectionStatusLabel.Text = status;
        end
        
        function results = openPort(app)
            status = app.serialPort.status;           
            if(status == 'open')
                %port already open
            else
                %open port
                port = app.COMPortDropDown.Value;
                baudRate = str2num(app.BaudRateDropDown.Value);
                app.serialPort = serial(port);
                app.serialPort.BaudRate = baudRate;
                fopen(app.serialPort);    
            end
            status = app.serialPort.status;
            app.ConnectionStatusLabel.Text = status;
        end
        
        %Read up until \n, returns line with \n discarded
        function results = readLine(app)
            pause(0.02);
            status = app.serialPort.status;
            data = 0;
            if(status == 'open')
                bytesAvailable = app.serialPort.BytesAvailable;
                %fprintf('Bytes available: ' + bytesAvailable + '\n');
                if(bytesAvailable > 0)
                    data =  fgetl(app.serialPort);
                    %fprintf(data + '\n');
                else
                    %fprintf('NO DATA\n');
                end
                    
            end
            
            results = data;          
        end
        
        %Refreshes the COM list displayed in the drop down
        function results = refreshCOMList(app)
            app.COMPortDropDown.Items = seriallist;  %get list of available serial ports
        end
    
        function results = testConnection(app)
            status = app.serialPort.status;
            if(status == 'open')
                %fprintf('testConnection\n');
                app.TestStatusLabel.Text = 'Testing...';
                app.writeLine('T'); %write a TEST\n to the port.
                pause(0.05);   %pause for a bit...
                if(app.readLine() == 'T_ACK')
                    app.TestStatusLabel.Text = 'Test Success';
                else
                    app.TestStatusLabel.Text = 'Test Failure';
                end
            else
                app.TestStatusLabel.Text = 'Port closed';
            end
        end
    
        %Send a line of text followed by \n
        function results = writeLine(app, data)     
            %fprintf('writeLine: ' + data + '\n');
            status = app.serialPort.status;
            if(status == 'open')
                fprintf(app.serialPort, data);                      
            end        
        end
        
        
        
        function results = clearBuffer(app)
            fprintf('clearBuffer\n');
            status = app.serialPort.status;
            if(status == 'open')
                flushinput(app.serialPort);                      
            end 
        end
        
        
        function results = writeFrequencyIncrement(app)
            
        end
        
        function results = getAnalogVoltage(app)
            app.writeLine('A');
            pause(0.02);
            
            data = 0;
            numberofbytes = 2;
            while(numberofbytes > 0)
                current_byte = double(app.readLine());
                current_byte = bitshift(current_byte, (numberofbytes-1)*8);
                data = bitor(data, current_byte);
                numberofbytes = numberofbytes - 1;
            end
            results = data*3.3/65536.0; 
            
        end
        
        function results = AnalogTempTimerFcn(app,~,~)
            analog_voltage = app.getAnalogVoltage();
            
            voltagedata = app.VdiffAbsoluteLine.YData;
            voltagedata = circshift(voltagedata,1);
            voltagedata(1) = analog_voltage;
            
            app.VdiffAbsoluteLine.YData = voltagedata;
            app.VdiffRelativeLine.YData = voltagedata - 1.65;
            app.AbsoluteLabel.Text = 'Absolute: ' + analog_voltage + ' V';
            app.RelativeLabel.Text = 'Relative: ' + (analog_voltage - 1.65) + ' V';
        end
        
        function results = setStartFrequency(app, startFrequency)
            startFrequency8 = typecast(uint32(startFrequency), 'uint8');
            low = startFrequency8(1);
            mid = startFrequency8(2);
            hi = startFrequency8(3);
            data_to_write = 'F' + string(char(hi)) + string(char(mid)) + string(char(low));
            
            app.writeLine(data_to_write);
        end
        
        function results = setFrequencyIncrement(app, frequencyIncrement)
            frequencyIncrement8 = typecast(uint32(frequencyIncrement), 'uint8');
            lo = frequencyIncrement8(1);
            mid = frequencyIncrement8(2);
            hi = frequencyIncrement8(3);
            data_to_write = 'I' + string(char(hi)) + string(char(mid)) + string(char(lo));
            app.writeLine(data_to_write);
        end
        
        function results = setNumberOfIncrements(app, numberOfIncrements)
            numberOfIncrements8 = typecast(uint16(numberOfIncrements), 'uint8');
            lo = numberOfIncrements8(1);
            hi = numberOfIncrements8(2);
            data_to_write = 'N' + string(char(hi)) + string(char(lo));
            app.writeLine(data_to_write);
        end
        %Factor should be 1, 2 or 4
        function results = setNumberOfSettlingCycles(app, settlingCycles, factor)
            settlingCycles8 = typecast(uint16(settlingCycles), 'uint8');
            lo = settlingCycles8(1);
            hi = settlingCycles8(2);
            data_to_write = 'C' + string(char(hi)) + string(char(lo)) + string(char(factor));
            app.writeLine(data_to_write);
        end
        
        function results = startFrequencySweep(app)
            output_range = double(app.range_1_button.Value*0) + double(app.range_2_button.Value*3) + double(app.range_3_button.Value*2) + double(app.range_4_button.Value*1);
            pga = double(app.PGA_x1_button.Value*1);
            data_to_write = 'S' + string(char(output_range)) + string(char(pga));
            app.writeLine(data_to_write);
        end
        
        function results = getSweepData(app, startFrequency, frequencyIncrement, numberOfIncrements)
            
        
            pos = 1;
            app.writeLine('P');    %request a printout of the data.
            pause(2);
            status = app.serialPort.status;
            data = 0;
            %clear all data.
            app.complexImpedanceData = [];
            app.magnitudeData = [];
            app.phaseData = [];
            app.frequencyArray = [];
            
            if(status == 'open')
                bytesAvailable = app.serialPort.BytesAvailable;
                %fprintf('Bytes available: ' + bytesAvailable + '\n');
                while(bytesAvailable > 0)
                    datastring =  fgetl(app.serialPort);     %This will be in the format '%d(real),%d(imag)
                    if(datastring == 'P_ACK')

                    else
                        dataPoint = strsplit(datastring, ',');
                        complexDataPoint = complex(str2double(dataPoint(1)), str2double(dataPoint(2)));
                        app.complexImpedanceData(pos) = complexDataPoint;
                        pos = pos + 1;
                    end
                    bytesAvailable = app.serialPort.BytesAvailable;
                end
                app.magnitudeData = abs(app.complexImpedanceData);
                app.phaseData = angle(app.complexImpedanceData);
                app.phaseData = unwrap(app.phaseData);
                
                for i = 1:numberOfIncrements + 1
                    app.frequencyArray(i) = startFrequency + ((i-1)*frequencyIncrement);
                end

 
            end
            %while
        end
        
    end
    

    methods (Access = private)

        % Code that executes after component creation
        function startupFcn(app)
            app.refreshCOMList();
            app.LocationEditField.Value = pwd;      %show the current location in the save box
      
            
            app.VdiffAbsoluteLine = plot(app.VdiffAbsoluteAxes,0:120,1.65*ones(1,121));
            app.VdiffRelativeLine = plot(app.VdiffRelativeAxes,0:120,zeros(1,121));
            
            app.AnalogTempTimer = timer(...
            'ExecutionMode', 'fixedRate', ...    % Run timer repeatedly
            'Period', 0.5, ...                     % Period is 1 second
            'BusyMode', 'queue',...              % Queue timer callbacks when busy
            'TimerFcn', @app.AnalogTempTimerFcn);    % Specify callback function
        end

        % Button pushed function: RefreshButton
        function RefreshButtonPushed(app, event)
            app.refreshCOMList();
        end

        % Button pushed function: OpenButton
        function OpenButtonPushed(app, event)
            app.openPort();  
        end

        % Close request function: AD5933ApplicationUIFigure
        function AD5933ApplicationUIFigureCloseRequest(app, event)

            stop(app.AnalogTempTimer);
            app.closePort();
            delete(app);
        end

        % Value changed function: FrequencyIncrementHzEditField, 
        % NumberofIncrementsEditField, StartFrequencyHzEditField
        function StartFrequencyHzEditFieldValueChanged(app, event)
            value = app.StartFrequencyHzEditField.Value;
            app.EndFrequencyHzEditField.Value = app.StartFrequencyHzEditField.Value + (app.FrequencyIncrementHzEditField.Value * app.NumberofIncrementsEditField.Value);
        end

        % Button pushed function: CloseButton
        function CloseButtonPushed(app, event)
            app.closePort();
        end

        % Button pushed function: TestConnectionButton
        function TestConnectionButtonPushed(app, event)
            %app.TestStatusLabel.Text = app.readLine();
            app.testConnection();
        end

        % Button pushed function: ClearBufferButton
        function ClearBufferButtonPushed(app, event)
            app.clearBuffer();
        end

        % Button pushed function: WritecontrolvaluesButton
        function WritecontrolvaluesButtonPushed(app, event)
            app.ControlstatusLabel.Text = 'Writing control values...';
            app.setStartFrequency(app.StartFrequencyHzEditField.Value);
            pause(0.1);
            app.setFrequencyIncrement(app.FrequencyIncrementHzEditField.Value);
            pause(0.1);
            app.setNumberOfIncrements(app.NumberofIncrementsEditField.Value);
            pause(0.1);
            settling_cycles_factor = double(app.x1Button.Value*1) + double(app.x2Button.Value*2) + double(app.x4Button.Value*4);
            app.setNumberOfSettlingCycles(app.NumberofSettlingCyclesEditField.Value, settling_cycles_factor);
            pause(0.1);
            app.ControlstatusLabel.Text = 'Control values written';
        end

        % Callback function
        function GetVdiffButtonPushed(app, event)
            analog_voltage = app.getAnalogVoltage();
            %analog_voltage
            app.VdiffAbsoluteField.Value = analog_voltage;
            app.VdiffabsoluteGauge.Value = analog_voltage;
            app.VdiffrelativeGauge.Value = analog_voltage - 1.65;
            app.VdiffRelativeField.Value = analog_voltage - 1.65;
        end

        % Button pushed function: StartrecordingButton
        function StartrecordingButtonPushed(app, event)
            if strcmp(app.AnalogTempTimer.Running, 'off')
               start(app.AnalogTempTimer);
            end
        end

        % Button pushed function: StoprecordingButton
        function StoprecordingButtonPushed(app, event)
            stop(app.AnalogTempTimer);
        end

        % Button pushed function: PerformcalibrationsweepButton
        function PerformcalibrationsweepButtonPushed(app, event)
            app.CalibrationstatusLabel.Text = 'Starting calibration sweep...';
            app.startFrequencySweep();
            pause(1);
            app.linearGainFactorArray = [];
            app.gainFactorArray = [];
            %poll for bytes available
            bytesAvailable = app.serialPort.BytesAvailable;
            while(bytesAvailable == 0)
                bytesAvailable = app.serialPort.BytesAvailable;
                pause(1);
            end
            if(app.readLine() == 'S_ACK')
                app.CalibrationstatusLabel.Text = 'Calibration complete';
            else
                app.CalibrationstatusLabel.Text = 'Calibration error';
            end
            
            calibration_impedance = app.CalibrationresistanceOhmsEditField.Value;
            app.getSweepData(app.StartFrequencyHzEditField.Value, app.FrequencyIncrementHzEditField.Value, app.NumberofIncrementsEditField.Value);
            
            app.gainFactorArray = 1./(calibration_impedance*app.magnitudeData);
            app.systemPhaseArray = app.phaseData;
            
            %app.meanSystemPhase = mean(app.phaseData);
            %calculate the gain factor at the extremes and plot a line.
            
            app.lowGainFactor = app.gainFactorArray(1);
            app.highGainFactor = app.gainFactorArray(app.NumberofIncrementsEditField.Value + 1);
            app.gainFactorVariation = app.highGainFactor - app.lowGainFactor;
            app.linearGainFactorArray = [];
            
            for i = 1:app.NumberofIncrementsEditField.Value + 1
                app.linearGainFactorArray(i) = app.lowGainFactor + ((i-1)*app.gainFactorVariation/app.NumberofIncrementsEditField.Value);
            end
            
            app.lowSystemPhase = app.systemPhaseArray(1);
            app.highSystemPhase = app.systemPhaseArray(app.NumberofIncrementsEditField.Value + 1);
            app.systemPhaseVariation = app.highSystemPhase - app.lowSystemPhase;
            app.linearSystemPhaseArray = [];
            for i = 1:app.NumberofIncrementsEditField.Value + 1
                app.linearSystemPhaseArray(i) = app.lowSystemPhase + ((i-1)*app.systemPhaseVariation/app.NumberofIncrementsEditField.Value);
            end
            
            cla(app.GainFactorAxes);
            cla(app.SystemPhaseAxes);
            
            
            
            app.LowGainFactorLabel.Text = 'Low Gain Factor: ' + app.lowGainFactor;
            app.HighGainFactorLabel.Text = 'High Gain Factor: ' + app.highGainFactor;
           
            hold(app.GainFactorAxes,'on');          
            scatter(app.GainFactorAxes,app.frequencyArray, app.gainFactorArray, 'blue');    %plot individual gain factors
            plot(app.GainFactorAxes, [app.frequencyArray(1) app.frequencyArray(app.NumberofIncrementsEditField.Value + 1)], [app.lowGainFactor app.highGainFactor], 'red');  %plot linear gain factor   
            hold(app.GainFactorAxes, 'off');
            
            hold(app.SystemPhaseAxes, 'on');
            scatter(app.SystemPhaseAxes,app.frequencyArray,app.systemPhaseArray);
            plot(app.SystemPhaseAxes, [app.frequencyArray(1) app.frequencyArray(app.NumberofIncrementsEditField.Value +1)], [app.lowSystemPhase app.highSystemPhase], 'red');
            hold(app.SystemPhaseAxes, 'off');
            %app.MeangainfactorLabel.Text = 'Mean Gain Factor: ' + string(app.meanGainFactor);
   
            %app.MeansystemphaseLabel.Text = 'Mean system phase: ' + string(app.meanSystemPhase) + ' rads';
            
        end

        % Button pushed function: PerformsweepButton
        function PerformsweepButtonPushed(app, event)
            app.SweepstatusLabel.Text = 'Starting frequency sweep...';
            app.startFrequencySweep();
            pause(1);
            
            cla(app.MagnitudeAxes);
            cla(app.PhaseAxes);
            
            %poll for bytes available
            bytesAvailable = app.serialPort.BytesAvailable;
            while(bytesAvailable == 0)
                bytesAvailable = app.serialPort.BytesAvailable;
                pause(1);
            end
            if(app.readLine() == 'S_ACK')
                app.SweepstatusLabel.Text = 'Frequency sweep complete';
            else
                app.SweepstatusLabel.Text = 'Frequency sweep error';
            end
            
            app.getSweepData(app.StartFrequencyHzEditField.Value, app.FrequencyIncrementHzEditField.Value, app.NumberofIncrementsEditField.Value);
            %scale the data using the system phase and gain factor
            app.impedanceDataArray = 1./(app.linearGainFactorArray.*app.magnitudeData);
            app.phaseData = app.phaseData - app.linearSystemPhaseArray;
            
            %this maps all the phases between pi and -pi
            for i = 1:1:app.NumberofIncrementsEditField.Value + 1
                if(app.phaseData(i) > pi)
                    app.phaseData(i) = app.phaseData(i) - (2*pi);
                end
                if(app.phaseData(i) < -pi)
                   app.phaseData(i) = app.phaseData(i) + (2*pi); 
                end
            end
            
            scatter(app.MagnitudeAxes,app.frequencyArray,app.impedanceDataArray);
            scatter(app.PhaseAxes,app.frequencyArray,app.phaseData);
        end

        % Button pushed function: BrowseButton
        function BrowseButtonPushed(app, event)
            app.saveLocation = uigetdir(app.LocationEditField.Value);
            if(app.saveLocation ~= 0)
                app.LocationEditField.Value = app.saveLocation;
            end
        end

        % Button pushed function: SaveButton
        function SaveButtonPushed(app, event)
            app.SavestatusLabel.Visible = 'on';
            
            if(string(app.FilenameEditField.Value) == '')
               app.SavestatusLabel.Text = 'Please enter a filename';
            end
            if(string(app.LocationEditField.Value) == '')
               app.SavestatusLabel.Text = 'Please specify a save location';
            end
            if((string(app.LocationEditField.Value) ~= '') && (string(app.FilenameEditField.Value) ~= ''))
                %good to save
                filename = string(app.LocationEditField.Value) + '\' + string(app.FilenameEditField.Value) + '.xlsx'
                dataTable = table(app.frequencyArray.', app.impedanceDataArray.', app.phaseData.');
                dataTable.Properties.VariableNames = {'Frequency_hz', 'Magnitude_ohms', 'Phase_rads'};
                writetable(dataTable, filename);
                app.SavestatusLabel.Text = 'Saved: ' + filename;
            end

        end
    end

    % App initialization and construction
    methods (Access = private)

        % Create UIFigure and components
        function createComponents(app)

            % Create AD5933ApplicationUIFigure
            app.AD5933ApplicationUIFigure = uifigure;
            app.AD5933ApplicationUIFigure.Position = [100 100 1000 700];
            app.AD5933ApplicationUIFigure.Name = 'AD5933 Application';
            app.AD5933ApplicationUIFigure.CloseRequestFcn = createCallbackFcn(app, @AD5933ApplicationUIFigureCloseRequest, true);

            % Create TabGroup
            app.TabGroup = uitabgroup(app.AD5933ApplicationUIFigure);
            app.TabGroup.Position = [1 1 1000 700];

            % Create COMPortSettingsTab
            app.COMPortSettingsTab = uitab(app.TabGroup);
            app.COMPortSettingsTab.Title = 'COM Port Settings';

            % Create OpenButton
            app.OpenButton = uibutton(app.COMPortSettingsTab, 'push');
            app.OpenButton.ButtonPushedFcn = createCallbackFcn(app, @OpenButtonPushed, true);
            app.OpenButton.Position = [17 574 100 22];
            app.OpenButton.Text = 'Open';

            % Create CloseButton
            app.CloseButton = uibutton(app.COMPortSettingsTab, 'push');
            app.CloseButton.ButtonPushedFcn = createCallbackFcn(app, @CloseButtonPushed, true);
            app.CloseButton.Position = [128 574 100 22];
            app.CloseButton.Text = 'Close';

            % Create TestConnectionButton
            app.TestConnectionButton = uibutton(app.COMPortSettingsTab, 'push');
            app.TestConnectionButton.ButtonPushedFcn = createCallbackFcn(app, @TestConnectionButtonPushed, true);
            app.TestConnectionButton.Position = [16 545 212 22];
            app.TestConnectionButton.Text = 'Test Connection';

            % Create ConnectionStatusLabel
            app.ConnectionStatusLabel = uilabel(app.COMPortSettingsTab);
            app.ConnectionStatusLabel.FontWeight = 'bold';
            app.ConnectionStatusLabel.Position = [17 523 211 22];
            app.ConnectionStatusLabel.Text = 'Connection Status';

            % Create COMPortDropDownLabel
            app.COMPortDropDownLabel = uilabel(app.COMPortSettingsTab);
            app.COMPortDropDownLabel.HorizontalAlignment = 'right';
            app.COMPortDropDownLabel.Position = [17 636 59 22];
            app.COMPortDropDownLabel.Text = 'COM Port';

            % Create COMPortDropDown
            app.COMPortDropDown = uidropdown(app.COMPortSettingsTab);
            app.COMPortDropDown.Items = {};
            app.COMPortDropDown.Position = [94 636 100 22];
            app.COMPortDropDown.Value = {};

            % Create BaudRateDropDownLabel
            app.BaudRateDropDownLabel = uilabel(app.COMPortSettingsTab);
            app.BaudRateDropDownLabel.HorizontalAlignment = 'right';
            app.BaudRateDropDownLabel.Enable = 'off';
            app.BaudRateDropDownLabel.Position = [17 604 62 22];
            app.BaudRateDropDownLabel.Text = 'Baud Rate';

            % Create BaudRateDropDown
            app.BaudRateDropDown = uidropdown(app.COMPortSettingsTab);
            app.BaudRateDropDown.Items = {'115200'};
            app.BaudRateDropDown.Enable = 'off';
            app.BaudRateDropDown.Position = [94 604 100 22];
            app.BaudRateDropDown.Value = '115200';

            % Create RefreshButton
            app.RefreshButton = uibutton(app.COMPortSettingsTab, 'push');
            app.RefreshButton.ButtonPushedFcn = createCallbackFcn(app, @RefreshButtonPushed, true);
            app.RefreshButton.Position = [206 636 100 22];
            app.RefreshButton.Text = 'Refresh';

            % Create TestStatusLabel
            app.TestStatusLabel = uilabel(app.COMPortSettingsTab);
            app.TestStatusLabel.FontWeight = 'bold';
            app.TestStatusLabel.Position = [17 502 211 22];
            app.TestStatusLabel.Text = 'TestStatus';

            % Create ClearBufferButton
            app.ClearBufferButton = uibutton(app.COMPortSettingsTab, 'push');
            app.ClearBufferButton.ButtonPushedFcn = createCallbackFcn(app, @ClearBufferButtonPushed, true);
            app.ClearBufferButton.Position = [17 470 100 22];
            app.ClearBufferButton.Text = {'Clear Buffer'; ''};

            % Create ImpedanceSpectroscopyTab
            app.ImpedanceSpectroscopyTab = uitab(app.TabGroup);
            app.ImpedanceSpectroscopyTab.Title = 'Impedance Spectroscopy';

            % Create ControlPanel
            app.ControlPanel = uipanel(app.ImpedanceSpectroscopyTab);
            app.ControlPanel.Title = 'Control';
            app.ControlPanel.Position = [20 315 359 340];

            % Create StartFrequencyHzEditFieldLabel
            app.StartFrequencyHzEditFieldLabel = uilabel(app.ControlPanel);
            app.StartFrequencyHzEditFieldLabel.HorizontalAlignment = 'right';
            app.StartFrequencyHzEditFieldLabel.Position = [13 247 117 22];
            app.StartFrequencyHzEditFieldLabel.Text = 'Start Frequency (Hz)';

            % Create StartFrequencyHzEditField
            app.StartFrequencyHzEditField = uieditfield(app.ControlPanel, 'numeric');
            app.StartFrequencyHzEditField.Limits = [0 Inf];
            app.StartFrequencyHzEditField.ValueDisplayFormat = '%.0f';
            app.StartFrequencyHzEditField.ValueChangedFcn = createCallbackFcn(app, @StartFrequencyHzEditFieldValueChanged, true);
            app.StartFrequencyHzEditField.HorizontalAlignment = 'left';
            app.StartFrequencyHzEditField.Position = [173 247 65 22];

            % Create FrequencyIncrementHzEditFieldLabel
            app.FrequencyIncrementHzEditFieldLabel = uilabel(app.ControlPanel);
            app.FrequencyIncrementHzEditFieldLabel.HorizontalAlignment = 'right';
            app.FrequencyIncrementHzEditFieldLabel.Position = [13 216 145 22];
            app.FrequencyIncrementHzEditFieldLabel.Text = 'Frequency Increment (Hz)';

            % Create FrequencyIncrementHzEditField
            app.FrequencyIncrementHzEditField = uieditfield(app.ControlPanel, 'numeric');
            app.FrequencyIncrementHzEditField.Limits = [0 Inf];
            app.FrequencyIncrementHzEditField.ValueDisplayFormat = '%.0f';
            app.FrequencyIncrementHzEditField.ValueChangedFcn = createCallbackFcn(app, @StartFrequencyHzEditFieldValueChanged, true);
            app.FrequencyIncrementHzEditField.HorizontalAlignment = 'left';
            app.FrequencyIncrementHzEditField.Position = [173 216 65 22];

            % Create NumberofIncrementsEditFieldLabel
            app.NumberofIncrementsEditFieldLabel = uilabel(app.ControlPanel);
            app.NumberofIncrementsEditFieldLabel.HorizontalAlignment = 'right';
            app.NumberofIncrementsEditFieldLabel.Position = [13 182 124 22];
            app.NumberofIncrementsEditFieldLabel.Text = 'Number of Increments';

            % Create NumberofIncrementsEditField
            app.NumberofIncrementsEditField = uieditfield(app.ControlPanel, 'numeric');
            app.NumberofIncrementsEditField.Limits = [0 511];
            app.NumberofIncrementsEditField.ValueDisplayFormat = '%.0f';
            app.NumberofIncrementsEditField.ValueChangedFcn = createCallbackFcn(app, @StartFrequencyHzEditFieldValueChanged, true);
            app.NumberofIncrementsEditField.HorizontalAlignment = 'left';
            app.NumberofIncrementsEditField.Position = [173 182 65 22];

            % Create NumberofSettlingCyclesEditFieldLabel
            app.NumberofSettlingCyclesEditFieldLabel = uilabel(app.ControlPanel);
            app.NumberofSettlingCyclesEditFieldLabel.HorizontalAlignment = 'right';
            app.NumberofSettlingCyclesEditFieldLabel.Position = [13 113 144 22];
            app.NumberofSettlingCyclesEditFieldLabel.Text = 'Number of Settling Cycles';

            % Create NumberofSettlingCyclesEditField
            app.NumberofSettlingCyclesEditField = uieditfield(app.ControlPanel, 'numeric');
            app.NumberofSettlingCyclesEditField.Limits = [0 511];
            app.NumberofSettlingCyclesEditField.ValueDisplayFormat = '%.0f';
            app.NumberofSettlingCyclesEditField.HorizontalAlignment = 'left';
            app.NumberofSettlingCyclesEditField.Position = [173 113 65 22];

            % Create EndFrequencyHzEditFieldLabel
            app.EndFrequencyHzEditFieldLabel = uilabel(app.ControlPanel);
            app.EndFrequencyHzEditFieldLabel.HorizontalAlignment = 'right';
            app.EndFrequencyHzEditFieldLabel.Enable = 'off';
            app.EndFrequencyHzEditFieldLabel.Position = [13 146 113 22];
            app.EndFrequencyHzEditFieldLabel.Text = 'End Frequency (Hz)';

            % Create EndFrequencyHzEditField
            app.EndFrequencyHzEditField = uieditfield(app.ControlPanel, 'numeric');
            app.EndFrequencyHzEditField.ValueDisplayFormat = '%.0f';
            app.EndFrequencyHzEditField.Editable = 'off';
            app.EndFrequencyHzEditField.HorizontalAlignment = 'left';
            app.EndFrequencyHzEditField.Enable = 'off';
            app.EndFrequencyHzEditField.Position = [173 146 65 22];

            % Create MCLKFrequencyHzEditFieldLabel
            app.MCLKFrequencyHzEditFieldLabel = uilabel(app.ControlPanel);
            app.MCLKFrequencyHzEditFieldLabel.HorizontalAlignment = 'right';
            app.MCLKFrequencyHzEditFieldLabel.Enable = 'off';
            app.MCLKFrequencyHzEditFieldLabel.Position = [13 279 125 22];
            app.MCLKFrequencyHzEditFieldLabel.Text = 'MCLK Frequency (Hz)';

            % Create MCLKFrequencyHzEditField
            app.MCLKFrequencyHzEditField = uieditfield(app.ControlPanel, 'numeric');
            app.MCLKFrequencyHzEditField.Limits = [0 Inf];
            app.MCLKFrequencyHzEditField.ValueDisplayFormat = '%.0f';
            app.MCLKFrequencyHzEditField.Editable = 'off';
            app.MCLKFrequencyHzEditField.HorizontalAlignment = 'left';
            app.MCLKFrequencyHzEditField.Enable = 'off';
            app.MCLKFrequencyHzEditField.Position = [173 279 64 22];
            app.MCLKFrequencyHzEditField.Value = 16000000;

            % Create WritecontrolvaluesButton
            app.WritecontrolvaluesButton = uibutton(app.ControlPanel, 'push');
            app.WritecontrolvaluesButton.ButtonPushedFcn = createCallbackFcn(app, @WritecontrolvaluesButtonPushed, true);
            app.WritecontrolvaluesButton.Position = [9 41 121 22];
            app.WritecontrolvaluesButton.Text = {'Write control values'; ''};

            % Create MultiplierButtonGroup
            app.MultiplierButtonGroup = uibuttongroup(app.ControlPanel);
            app.MultiplierButtonGroup.Title = 'Multiplier';
            app.MultiplierButtonGroup.Position = [245 41 100 94];

            % Create x1Button
            app.x1Button = uiradiobutton(app.MultiplierButtonGroup);
            app.x1Button.Text = 'x1';
            app.x1Button.Position = [11 48 58 22];
            app.x1Button.Value = true;

            % Create x2Button
            app.x2Button = uiradiobutton(app.MultiplierButtonGroup);
            app.x2Button.Text = 'x2';
            app.x2Button.Position = [11 26 65 22];

            % Create x4Button
            app.x4Button = uiradiobutton(app.MultiplierButtonGroup);
            app.x4Button.Text = 'x4';
            app.x4Button.Position = [11 4 65 22];

            % Create ControlstatusLabel
            app.ControlstatusLabel = uilabel(app.ControlPanel);
            app.ControlstatusLabel.FontWeight = 'bold';
            app.ControlstatusLabel.Position = [13 12 225 22];
            app.ControlstatusLabel.Text = 'Control status';

            % Create TabGroup2
            app.TabGroup2 = uitabgroup(app.ImpedanceSpectroscopyTab);
            app.TabGroup2.Position = [405 118 582 537];

            % Create MagnitudeTab
            app.MagnitudeTab = uitab(app.TabGroup2);
            app.MagnitudeTab.Title = 'Magnitude';

            % Create MagnitudeAxes
            app.MagnitudeAxes = uiaxes(app.MagnitudeTab);
            title(app.MagnitudeAxes, 'Magnitude')
            xlabel(app.MagnitudeAxes, 'Frequency /Hz')
            ylabel(app.MagnitudeAxes, 'Impedance magnitude')
            app.MagnitudeAxes.Position = [1 1 579 511];

            % Create PhaseTab
            app.PhaseTab = uitab(app.TabGroup2);
            app.PhaseTab.Title = 'Phase';

            % Create PhaseAxes
            app.PhaseAxes = uiaxes(app.PhaseTab);
            title(app.PhaseAxes, 'Phase')
            xlabel(app.PhaseAxes, 'Frequency /Hz')
            ylabel(app.PhaseAxes, 'Phase /rads')
            app.PhaseAxes.PlotBoxAspectRatio = [1 0.900596446532552 0.900596446532552];
            app.PhaseAxes.YLim = [-3.14159265358979 3.14159265358979];
            app.PhaseAxes.YTick = [-3.14159265358979 -1.5707963267949 0 1.5707963267949 3.14159265358979];
            app.PhaseAxes.YGrid = 'on';
            app.PhaseAxes.Position = [1 1 579 511];

            % Create PerformsweepButton
            app.PerformsweepButton = uibutton(app.ImpedanceSpectroscopyTab, 'push');
            app.PerformsweepButton.ButtonPushedFcn = createCallbackFcn(app, @PerformsweepButtonPushed, true);
            app.PerformsweepButton.Position = [21 119 100 22];
            app.PerformsweepButton.Text = 'Perform sweep';

            % Create SweepstatusLabel
            app.SweepstatusLabel = uilabel(app.ImpedanceSpectroscopyTab);
            app.SweepstatusLabel.FontWeight = 'bold';
            app.SweepstatusLabel.Position = [132 119 233 22];
            app.SweepstatusLabel.Text = {'Sweep status'; ''};

            % Create PGAGainButtonGroup
            app.PGAGainButtonGroup = uibuttongroup(app.ImpedanceSpectroscopyTab);
            app.PGAGainButtonGroup.Title = 'PGA Gain';
            app.PGAGainButtonGroup.Position = [164 165 123 130];

            % Create PGA_x1_button
            app.PGA_x1_button = uiradiobutton(app.PGAGainButtonGroup);
            app.PGA_x1_button.Text = 'x1';
            app.PGA_x1_button.Position = [11 84 58 22];
            app.PGA_x1_button.Value = true;

            % Create PGA_x5_button
            app.PGA_x5_button = uiradiobutton(app.PGAGainButtonGroup);
            app.PGA_x5_button.Text = 'x5';
            app.PGA_x5_button.Position = [11 62 65 22];

            % Create OutputRangeButtonGroup
            app.OutputRangeButtonGroup = uibuttongroup(app.ImpedanceSpectroscopyTab);
            app.OutputRangeButtonGroup.Title = 'Output Range';
            app.OutputRangeButtonGroup.Position = [20 165 136 130];

            % Create range_1_button
            app.range_1_button = uiradiobutton(app.OutputRangeButtonGroup);
            app.range_1_button.Text = '1: 2.0 Vpp';
            app.range_1_button.Position = [11 84 77 22];
            app.range_1_button.Value = true;

            % Create range_2_button
            app.range_2_button = uiradiobutton(app.OutputRangeButtonGroup);
            app.range_2_button.Text = '2: 1.0 Vpp';
            app.range_2_button.Position = [11 62 77 22];

            % Create range_3_button
            app.range_3_button = uiradiobutton(app.OutputRangeButtonGroup);
            app.range_3_button.Text = '3: 400 mVpp';
            app.range_3_button.Position = [11 40 90 22];

            % Create range_4_button
            app.range_4_button = uiradiobutton(app.OutputRangeButtonGroup);
            app.range_4_button.Text = '4: 200 mVpp';
            app.range_4_button.Position = [11 11 90 22];

            % Create SaveButton
            app.SaveButton = uibutton(app.ImpedanceSpectroscopyTab, 'push');
            app.SaveButton.ButtonPushedFcn = createCallbackFcn(app, @SaveButtonPushed, true);
            app.SaveButton.Position = [406 19 100 22];
            app.SaveButton.Text = 'Save';

            % Create FilenameEditFieldLabel
            app.FilenameEditFieldLabel = uilabel(app.ImpedanceSpectroscopyTab);
            app.FilenameEditFieldLabel.HorizontalAlignment = 'right';
            app.FilenameEditFieldLabel.Position = [405 54 58 22];
            app.FilenameEditFieldLabel.Text = 'File name';

            % Create FilenameEditField
            app.FilenameEditField = uieditfield(app.ImpedanceSpectroscopyTab, 'text');
            app.FilenameEditField.Position = [478 54 186 22];

            % Create LocationEditFieldLabel
            app.LocationEditFieldLabel = uilabel(app.ImpedanceSpectroscopyTab);
            app.LocationEditFieldLabel.HorizontalAlignment = 'right';
            app.LocationEditFieldLabel.Position = [405 87 51 22];
            app.LocationEditFieldLabel.Text = 'Location';

            % Create LocationEditField
            app.LocationEditField = uieditfield(app.ImpedanceSpectroscopyTab, 'text');
            app.LocationEditField.Position = [478 87 186 22];

            % Create BrowseButton
            app.BrowseButton = uibutton(app.ImpedanceSpectroscopyTab, 'push');
            app.BrowseButton.ButtonPushedFcn = createCallbackFcn(app, @BrowseButtonPushed, true);
            app.BrowseButton.Position = [689 87 100 22];
            app.BrowseButton.Text = 'Browse';

            % Create xlsxLabel
            app.xlsxLabel = uilabel(app.ImpedanceSpectroscopyTab);
            app.xlsxLabel.Position = [671 54 29 22];
            app.xlsxLabel.Text = '.xlsx';

            % Create SavestatusLabel
            app.SavestatusLabel = uilabel(app.ImpedanceSpectroscopyTab);
            app.SavestatusLabel.Position = [518 19 467 22];
            app.SavestatusLabel.Text = 'Save status';

            % Create CalibrationTab
            app.CalibrationTab = uitab(app.TabGroup);
            app.CalibrationTab.Title = 'Calibration';

            % Create PerformcalibrationsweepButton
            app.PerformcalibrationsweepButton = uibutton(app.CalibrationTab, 'push');
            app.PerformcalibrationsweepButton.ButtonPushedFcn = createCallbackFcn(app, @PerformcalibrationsweepButtonPushed, true);
            app.PerformcalibrationsweepButton.Position = [32 585 279 22];
            app.PerformcalibrationsweepButton.Text = 'Perform calibration sweep';

            % Create GainFactorAxes
            app.GainFactorAxes = uiaxes(app.CalibrationTab);
            title(app.GainFactorAxes, 'Gain factor')
            xlabel(app.GainFactorAxes, 'Frequency')
            ylabel(app.GainFactorAxes, 'Gain Factor')
            app.GainFactorAxes.PlotBoxAspectRatio = [1 0.433898305084746 0.433898305084746];
            app.GainFactorAxes.Position = [32 254 345 186];

            % Create CalibrationstatusLabel
            app.CalibrationstatusLabel = uilabel(app.CalibrationTab);
            app.CalibrationstatusLabel.FontWeight = 'bold';
            app.CalibrationstatusLabel.Position = [321 585 202 22];
            app.CalibrationstatusLabel.Text = 'Calibration status';

            % Create MeangainfactorLabel
            app.MeangainfactorLabel = uilabel(app.CalibrationTab);
            app.MeangainfactorLabel.Position = [32 519 264 22];
            app.MeangainfactorLabel.Text = 'Mean gain factor:';

            % Create SystemPhaseAxes
            app.SystemPhaseAxes = uiaxes(app.CalibrationTab);
            title(app.SystemPhaseAxes, 'System phase')
            xlabel(app.SystemPhaseAxes, 'Frequency')
            ylabel(app.SystemPhaseAxes, 'Phase /rads')
            app.SystemPhaseAxes.PlotBoxAspectRatio = [1 0.475836444468121 0.475836444468121];
            app.SystemPhaseAxes.YLim = [-3.14159265358979 3.14159265358979];
            app.SystemPhaseAxes.YTick = [-3.14159265358979 -1.5707963267949 0 1.5707963267949 3.14159265358979];
            app.SystemPhaseAxes.YGrid = 'on';
            app.SystemPhaseAxes.Position = [402 254 345 186];

            % Create MeansystemphaseLabel
            app.MeansystemphaseLabel = uilabel(app.CalibrationTab);
            app.MeansystemphaseLabel.Position = [402 519 270 22];
            app.MeansystemphaseLabel.Text = 'Mean system phase:';

            % Create CalibrationresistanceOhmsEditFieldLabel
            app.CalibrationresistanceOhmsEditFieldLabel = uilabel(app.CalibrationTab);
            app.CalibrationresistanceOhmsEditFieldLabel.HorizontalAlignment = 'right';
            app.CalibrationresistanceOhmsEditFieldLabel.Position = [32 616 164 22];
            app.CalibrationresistanceOhmsEditFieldLabel.Text = 'Calibration resistance (Ohms)';

            % Create CalibrationresistanceOhmsEditField
            app.CalibrationresistanceOhmsEditField = uieditfield(app.CalibrationTab, 'numeric');
            app.CalibrationresistanceOhmsEditField.Limits = [0 Inf];
            app.CalibrationresistanceOhmsEditField.Position = [211 616 100 22];

            % Create LowGainFactorLabel
            app.LowGainFactorLabel = uilabel(app.CalibrationTab);
            app.LowGainFactorLabel.Position = [32 274 371 22];
            app.LowGainFactorLabel.Text = 'Low Gain Factor';

            % Create HighGainFactorLabel
            app.HighGainFactorLabel = uilabel(app.CalibrationTab);
            app.HighGainFactorLabel.Position = [32 243 371 22];
            app.HighGainFactorLabel.Text = 'High Gain Factor';

            % Create FlowRateTab
            app.FlowRateTab = uitab(app.TabGroup);
            app.FlowRateTab.Title = 'Flow Rate';

            % Create VdiffAbsoluteAxes
            app.VdiffAbsoluteAxes = uiaxes(app.FlowRateTab);
            title(app.VdiffAbsoluteAxes, 'Vdiff (absolute)')
            xlabel(app.VdiffAbsoluteAxes, 'Time /s')
            ylabel(app.VdiffAbsoluteAxes, 'Voltage /V')
            app.VdiffAbsoluteAxes.PlotBoxAspectRatio = [1 0.360534119419502 0.360534119419502];
            app.VdiffAbsoluteAxes.YLim = [0 3.3];
            app.VdiffAbsoluteAxes.XTick = [];
            app.VdiffAbsoluteAxes.YTick = [0 0.5 1 1.5 2 2.5 3 3.3];
            app.VdiffAbsoluteAxes.YGrid = 'on';
            app.VdiffAbsoluteAxes.Position = [368 305 600 279];

            % Create VdiffRelativeAxes
            app.VdiffRelativeAxes = uiaxes(app.FlowRateTab);
            title(app.VdiffRelativeAxes, 'Vdiff (relative to 1.65V)')
            xlabel(app.VdiffRelativeAxes, 'Time /s')
            ylabel(app.VdiffRelativeAxes, 'Voltage /V')
            app.VdiffRelativeAxes.PlotBoxAspectRatio = [1 0.367069481100823 0.367069481100823];
            app.VdiffRelativeAxes.YLim = [-1.65 1.65];
            app.VdiffRelativeAxes.XTick = [];
            app.VdiffRelativeAxes.YTick = [-1.65 -1 -0.5 0 0.5 1 1.65];
            app.VdiffRelativeAxes.YGrid = 'on';
            app.VdiffRelativeAxes.Position = [368 21 600 285];

            % Create StartrecordingButton
            app.StartrecordingButton = uibutton(app.FlowRateTab, 'push');
            app.StartrecordingButton.ButtonPushedFcn = createCallbackFcn(app, @StartrecordingButtonPushed, true);
            app.StartrecordingButton.Position = [24 623 100 22];
            app.StartrecordingButton.Text = 'Start recording';

            % Create StoprecordingButton
            app.StoprecordingButton = uibutton(app.FlowRateTab, 'push');
            app.StoprecordingButton.ButtonPushedFcn = createCallbackFcn(app, @StoprecordingButtonPushed, true);
            app.StoprecordingButton.Position = [24 587 100 22];
            app.StoprecordingButton.Text = 'Stop recording';

            % Create AbsoluteLabel
            app.AbsoluteLabel = uilabel(app.FlowRateTab);
            app.AbsoluteLabel.FontSize = 36;
            app.AbsoluteLabel.Position = [24 499 345 45];
            app.AbsoluteLabel.Text = 'Absolute:';

            % Create RelativeLabel
            app.RelativeLabel = uilabel(app.FlowRateTab);
            app.RelativeLabel.FontSize = 36;
            app.RelativeLabel.Position = [24 255 345 45];
            app.RelativeLabel.Text = 'Relative:';
        end
    end

    methods (Access = public)

        % Construct app
        function app = boardGUI

            % Create and configure components
            createComponents(app)

            % Register the app with App Designer
            registerApp(app, app.AD5933ApplicationUIFigure)

            % Execute the startup function
            runStartupFcn(app, @startupFcn)

            if nargout == 0
                clear app
            end
        end

        % Code that executes before app deletion
        function delete(app)

            % Delete UIFigure when app is deleted
            delete(app.AD5933ApplicationUIFigure)
        end
    end
end
