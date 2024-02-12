const express = require('express');
const bodyParser = require('body-parser');
const mongoose = require('mongoose');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(bodyParser.json())
app.use((req, res, next) => {
    res.setHeader('Access-Control-Allow-Origin', 'http://127.0.0.1:8080'); // Установите здесь домен вашего фронтенда
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, PATCH, DELETE');
    res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');
    res.setHeader('Access-Control-Allow-Credentials', true);
    next();
});

mongoose.connect('mongodb://localhost:27017/weather-app');

const WeatherDataSchema = new mongoose.Schema({
    temperature: Number,
    humidity: Number,
    light: Number,
    pressure: Number,
    date: {type: Date, default: Date.now()}
});

const WeatherData = mongoose.model('WeatherData', WeatherDataSchema);

app.post('/weather', async (req, res) => {
    try{
        const {temperature, humidity, light, pressure} = req.body;
        const weatherData = new WeatherData({
            temperature,
            humidity,
            light,
            pressure
        });
        await weatherData.save();
        res.status(201).send('Weather data saved successfully!');
    } catch (error){
        console.error(error);
        res.status(500).send('Error saving weather data.');
    }
});

app.get('/getWeather', async (req, res)=> {
    try{
        const weatherData = await WeatherData.find().sort({date: 'desc'}).exec();
        res.json(weatherData);
    }catch (error){
        console.error(error);
        res.status(500).send('Error fetching weather data.');
    }
})

app.listen(PORT, ()=> {
    console.log(`Server is running on port ${PORT}`)
})
