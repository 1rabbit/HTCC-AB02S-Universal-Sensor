function Decoder(bytes, port)
{
    var decoded = {};
    decoded.device = "rabbit-tracker";
    if (port > 12) decoded.device = "rabbit-sensor";
    
    //////////////  TRACKER  ////////////// 
    
    var latitude = ((bytes[0]<<16)>>>0) + ((bytes[1]<<8)>>>0) + bytes[2];
    latitude = parseFloat(((latitude / 16777215.0 * 180) - 90).toFixed(6));
    
    var longitude = ((bytes[3]<<16)>>>0) + ((bytes[4]<<8)>>>0) + bytes[5];
    longitude = parseFloat(((longitude / 16777215.0 * 360) - 180).toFixed(6));
    
    var altValue = ((bytes[6]<<8)>>>0) + bytes[7];
    var sign = bytes[6] & (1 << 7);
    
    if (port == 2) decoded.accuracy = 2.5;
    
    if (port == 2 || port == 3)
    {
        decoded.latitude = latitude;
        decoded.longitude = longitude; 
            
        if(sign) decoded.altitude = 0xFFFF0000 | altValue;
        else decoded.altitude = altValue;
            
        decoded.battery = parseFloat((bytes[8]/100 + 2).toFixed(2));
        decoded.sats = bytes[9];
        decoded.hdop = bytes[10]/10;
            
        if (typeof bytes[11] !== 'undefined') decoded.temperature = parseFloat((bytes[11]/5).toFixed(1));
            
    }
    else if (port == 4)
    {
        decoded.sats = 0;
        decoded.latitude = latitude;
        decoded.longitude = longitude; 
        decoded.battery = parseFloat((bytes[6]/100 + 2).toFixed(2));
            
        if (typeof bytes[7] !== 'undefined') decoded.temperature = parseFloat((bytes[7]/5).toFixed(1));
    }
    else if (port >= 5 && port <=  11)
	{
        decoded.sats = 0;
        decoded.battery = parseFloat((bytes[0]/100 + 2).toFixed(2));
            
        if (typeof bytes[1] !== 'undefined') decoded.temperature = parseFloat((bytes[1]/5).toFixed(1));
    }
    else if (port == 12)
	{
        decoded.nonstop = bytes[0]
        decoded.distance = bytes[1]
        decoded.screen = bytes[2]
        decoded.autosleep = bytes[3]
        decoded.gpswait = bytes[4]
        decoded.sleepupdate = bytes[5]
        decoded.deepsleep = bytes[6]
            
        if (decoded.screen == 0) decoded.screen = 1
        else                 decoded.screen = 0
    }
    
    // Voltage to Percent Generic Li-Ion
    if (typeof decoded.battery !== 'undefined')
    {
        VoltageToPercent = [{"voltage":4.1,"percent":100},{"voltage":4.09,"percent":99},{"voltage":4.08,"percent":98},{"voltage":4.07,"percent":97},{"voltage":4.07,"percent":96},{"voltage":4.06,"percent":95},{"voltage":4.05,"percent":94},{"voltage":4.04,"percent":93},{"voltage":4.04,"percent":92},{"voltage":4.03,"percent":91},{"voltage":4.02,"percent":90},{"voltage":4.01,"percent":90},{"voltage":4,"percent":89},{"voltage":4,"percent":88},{"voltage":3.99,"percent":87},{"voltage":3.98,"percent":86},{"voltage":3.97,"percent":85},{"voltage":3.97,"percent":84},{"voltage":3.96,"percent":83},{"voltage":3.95,"percent":82},{"voltage":3.94,"percent":81},{"voltage":3.94,"percent":81},{"voltage":3.93,"percent":80},{"voltage":3.92,"percent":79},{"voltage":3.91,"percent":78},{"voltage":3.9,"percent":77},{"voltage":3.9,"percent":76},{"voltage":3.89,"percent":75},{"voltage":3.88,"percent":74},{"voltage":3.87,"percent":73},{"voltage":3.86,"percent":72},{"voltage":3.85,"percent":71},{"voltage":3.84,"percent":71},{"voltage":3.84,"percent":70},{"voltage":3.83,"percent":69},{"voltage":3.82,"percent":68},{"voltage":3.81,"percent":67},{"voltage":3.81,"percent":66},{"voltage":3.8,"percent":65},{"voltage":3.79,"percent":64},{"voltage":3.79,"percent":63},{"voltage":3.78,"percent":62},{"voltage":3.77,"percent":62},{"voltage":3.77,"percent":61},{"voltage":3.76,"percent":60},{"voltage":3.75,"percent":59},{"voltage":3.75,"percent":58},{"voltage":3.74,"percent":57},{"voltage":3.74,"percent":56},{"voltage":3.73,"percent":55},{"voltage":3.72,"percent":54},{"voltage":3.72,"percent":53},{"voltage":3.71,"percent":52},{"voltage":3.71,"percent":52},{"voltage":3.7,"percent":51},{"voltage":3.69,"percent":50},{"voltage":3.69,"percent":49},{"voltage":3.68,"percent":48},{"voltage":3.68,"percent":47},{"voltage":3.67,"percent":46},{"voltage":3.67,"percent":45},{"voltage":3.66,"percent":44},{"voltage":3.66,"percent":43},{"voltage":3.65,"percent":42},{"voltage":3.65,"percent":42},{"voltage":3.64,"percent":41},{"voltage":3.64,"percent":40},{"voltage":3.63,"percent":39},{"voltage":3.63,"percent":38},{"voltage":3.63,"percent":37},{"voltage":3.62,"percent":36},{"voltage":3.62,"percent":35},{"voltage":3.61,"percent":34},{"voltage":3.61,"percent":33},{"voltage":3.6,"percent":33},{"voltage":3.6,"percent":32},{"voltage":3.59,"percent":31},{"voltage":3.59,"percent":30},{"voltage":3.58,"percent":29},{"voltage":3.58,"percent":28},{"voltage":3.57,"percent":27},{"voltage":3.57,"percent":26},{"voltage":3.56,"percent":25},{"voltage":3.56,"percent":24},{"voltage":3.55,"percent":23},{"voltage":3.54,"percent":23},{"voltage":3.54,"percent":22},{"voltage":3.53,"percent":21},{"voltage":3.53,"percent":20},{"voltage":3.52,"percent":19},{"voltage":3.51,"percent":18},{"voltage":3.5,"percent":17},{"voltage":3.5,"percent":16},{"voltage":3.49,"percent":15},{"voltage":3.49,"percent":14},{"voltage":3.49,"percent":13},{"voltage":3.48,"percent":13},{"voltage":3.48,"percent":12},{"voltage":3.48,"percent":11},{"voltage":3.47,"percent":10},{"voltage":3.47,"percent":9},{"voltage":3.46,"percent":8},{"voltage":3.45,"percent":7},{"voltage":3.44,"percent":6},{"voltage":3.43,"percent":5},{"voltage":3.41,"percent":4},{"voltage":3.39,"percent":4},{"voltage":3.37,"percent":3},{"voltage":3.35,"percent":2},{"voltage":3.33,"percent":1},{"voltage":3.3,"percent":0}]
        decoded.capacity = 100
        
        VoltageToPercent.forEach( (element, index) =>
        {
            if (VoltageToPercent[index].voltage >= decoded.battery)
            {
                decoded.capacity = VoltageToPercent[index].percent
            }
        })
    }


    //////////////  SENSORS  ////////////// 
        
    else if (port == 20)
	{
        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        if (typeof bytes[3] !== 'undefined') decoded.temperature = parseFloat((((bytes[2] << 8) + bytes[3] - 10000) / 100).toFixed(2));
        
        if (typeof bytes[5] !== 'undefined') decoded.temperature2 = parseFloat((((bytes[4] << 8) + bytes[5] - 10000) / 100).toFixed(2));
    }
            
            
    else if (port == 21)
	{
        decoded.device = "rabbit-tracker";
            
        decoded.sats = bytes[8];
            
        if (decoded.sats > 0)
        {
            decoded.latitude = latitude;
            decoded.longitude = longitude; 
    
            if(sign) decoded.altitude = 0xFFFF0000 | altValue;
            else decoded.altitude = altValue;
        }

        decoded.battery = parseFloat((((bytes[9] * 20) + 1500) / 1000).toFixed(2));
            
        if (typeof bytes[10] !== 'undefined') decoded.temperature = parseFloat(( (bytes[10]/2)-55 ).toFixed(1));
    }
    else if (port == 22)
	{
        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        if (typeof bytes[3] !== 'undefined') decoded.temperature0 = parseFloat((((bytes[2] << 8) + bytes[3] - 10000) / 100).toFixed(2));
        
        if (typeof bytes[5] !== 'undefined') decoded.humidity = parseFloat((((bytes[4] << 8) + bytes[5]) / 100).toFixed(2));
        
        if (typeof bytes[7] !== 'undefined') decoded.temperature = parseFloat((((bytes[6] << 8) + bytes[7] - 10000) / 100).toFixed(2));
        
        if (typeof bytes[9] !== 'undefined') decoded.temperature2 = parseFloat((((bytes[8] << 8) + bytes[9] - 10000) / 100).toFixed(2));
    }

    else if (port == 23)
	{ 

        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        decoded.adc3 = parseFloat((((bytes[2] << 8) + bytes[3])).toFixed(3));
            
        decoded.soil0 = -decoded.adc3 + 4095
            
        decoded.soil0 = decoded.soil0 - 200
            
        if (decoded.soil0 < 0) decoded.soil0 = 0
            
        decoded.soil0 = decoded.soil0 * (100 / 2350)
            
        decoded.soil0 = parseFloat(decoded.soil0.toFixed(1))
        
        //node.warn(decoded.adc3)
        //node.warn(decoded.soil0)
        
        if (typeof bytes[7] !== 'undefined') decoded.temperature0 = parseFloat((((bytes[6] << 8) + bytes[7] - 10000) / 100).toFixed(2));
        
        if (typeof bytes[9] !== 'undefined') decoded.humidity = parseFloat((((bytes[8] << 8) + bytes[9]) / 100).toFixed(2));

    }

    else if (port == 24)
	{
	    decoded.device = "BME280";

        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        if (typeof bytes[3] !== 'undefined') decoded.temperature = parseFloat((((bytes[2] << 8) + bytes[3] - 10000) / 100).toFixed(2));
        
        if (typeof bytes[5] !== 'undefined') decoded.humidity = parseFloat((((bytes[4] << 8) + bytes[5]) / 100).toFixed(2));
        
        if (typeof bytes[7] !== 'undefined') decoded.pressure = parseFloat((((bytes[6] << 8) + bytes[7]) / 10).toFixed(6));
    }

    else if (port == 30)
	{
        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
        decoded.temperature = parseFloat((((bytes[2] << 8) + bytes[3] - 10000) / 100).toFixed(2));
        decoded.pressure = parseFloat((((bytes[4] << 8) + bytes[5]) / 10).toFixed(6));
    }
        
        
    else if (port == 31)
	{
        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
        decoded.lux = parseFloat(((bytes[2] << 8) + bytes[3]).toFixed(0));
    }
        
        
    else if (port == 40 || port == 99)
	{
		decoded.device = "thermostat";

        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        decoded.temperature = parseFloat((((bytes[2] << 8) + bytes[3] - 10000) / 100).toFixed(2));
            
        decoded.humidity = parseFloat((((bytes[4] << 8) + bytes[5]) / 100).toFixed(2));
            
        decoded.set_temperature = parseFloat((bytes[6] / 10).toFixed(2));
            
        decoded.heating = bytes[7];
    }
        
    else if (port == 50)
	{ 
        decoded.device = "SDS011";

        //decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
            
        decoded.pm25 = parseFloat((((bytes[2] << 8) + bytes[3]) / 10).toFixed(1));
            
        decoded.pm10 = parseFloat((((bytes[4] << 8) + bytes[5]) / 10).toFixed(1));

        if (typeof bytes[7] !== 'undefined')
        {
            decoded.duty_cycle = bytes[6];
            decoded.clean_secs = bytes[7];
        }
    }
    else
    {
        decoded = { "bytes": bytes }
        decoded.battery = parseFloat((((bytes[0] << 8) + bytes[1]) / 1000).toFixed(3));
    }
    
    return decoded
}


