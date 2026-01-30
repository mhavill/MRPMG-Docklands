
const char animations_page_html[] PROGMEM = R"rawSrting(
    <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Animations</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        
        .container {
            background: white;
            border-radius: 16px;
            box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
            padding: 40px 30px;
            max-width: 500px;
            width: 100%;
            text-align: center;
        }
        
        h1 {
            color: #667eea;
            font-size: 28px;
            margin-bottom: 15px;
        }
        
        .status {
            background: #f7fafc;
            border-left: 4px solid #48bb78;
            padding: 15px;
            border-radius: 6px;
            margin-bottom: 30px;
        }
        
        .status p {
            color: #2d3748;
            font-size: 16px;
            margin: 0;
        }
        
        .icon {
            font-size: 64px;
            margin: 20px 0;
            animation: pulse 2s ease-in-out infinite;
        }
        
        @keyframes pulse {
            0%, 100% {
                transform: scale(1);
                opacity: 1;
            }
            50% {
                transform: scale(1.1);
                opacity: 0.8;
            }
        }
        
        .stop-button {
            width: 100%;
            padding: 18px;
            background: #e53e3e;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 18px;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            margin-top: 20px;
        }
        
        .stop-button:hover {
            background: #c53030;
            transform: translateY(-2px);
            box-shadow: 0 10px 20px rgba(229, 62, 62, 0.3);
        }
        
        .stop-button:active {
            transform: translateY(0);
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 30px 20px;
            }
            
            h1 {
                font-size: 24px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Animations Running</h1>
        
        <div class="status">
            <p><strong>Status:</strong> Animation patterns are now playing</p>
        </div>
        
        <div class="icon">✨</div>
        
        <form action="/setmode" method="POST">
            <input type="hidden" name="mode" value="stop">
            <button type="submit" class="stop-button">⬛ STOP & Return to Menu</button>
        </form>
    </div>
</body>
</html>
)rawSrting";