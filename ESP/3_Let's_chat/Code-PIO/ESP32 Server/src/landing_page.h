const char landing_page_html[] PROGMEM = R"rawString(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Display Controller</title>
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
        }
        
        .header {
            text-align: center;
            margin-bottom: 35px;
        }
        
        h1 {
            color: #667eea;
            font-size: 28px;
            margin-bottom: 10px;
        }
        
        .subtitle {
            color: #718096;
            font-size: 16px;
        }
        
        .mode-buttons {
            display: flex;
            flex-direction: column;
            gap: 15px;
        }
        
        .mode-button {
            width: 100%;
            padding: 20px;
            background: white;
            border: 3px solid #e2e8f0;
            border-radius: 12px;
            font-size: 18px;
            font-weight: 600;
            color: #2d3748;
            cursor: pointer;
            transition: all 0.3s ease;
            display: flex;
            align-items: center;
            justify-content: space-between;
        }
        
        .mode-button:hover {
            border-color: #667eea;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            transform: translateY(-2px);
            box-shadow: 0 10px 20px rgba(102, 126, 234, 0.3);
        }
        
        .mode-button:active {
            transform: translateY(0);
        }
        
        .mode-icon {
            font-size: 24px;
            margin-right: 15px;
        }
        
        .mode-text {
            flex: 1;
            text-align: left;
        }
        
        .mode-description {
            font-size: 14px;
            font-weight: 400;
            opacity: 0.8;
            margin-top: 5px;
        }
        
        .arrow {
            font-size: 20px;
            opacity: 0.5;
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 30px 20px;
            }
            
            h1 {
                font-size: 24px;
            }
            
            .mode-button {
                padding: 18px;
                font-size: 16px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32 Display Controller</h1>
            <p class="subtitle">Choose a display mode</p>
        </div>
        
        <div class="mode-buttons">
            <form action="/setmode" method="POST" style="margin: 0;">
                <input type="hidden" name="mode" value="text">
                <button type="submit" class="mode-button">
                    <span class="mode-icon">💬</span>
                    <div class="mode-text">
                        <div>Text Messages</div>
                        <div class="mode-description">Display scrolling text messages</div>
                    </div>
                    <span class="arrow">→</span>
                </button>
            </form>
            
            <form action="/setmode" method="POST" style="margin: 0;">
                <input type="hidden" name="mode" value="animations">
                <button type="submit" class="mode-button">
                    <span class="mode-icon">✨</span>
                    <div class="mode-text">
                        <div>Animations</div>
                        <div class="mode-description">Run animation patterns</div>
                    </div>
                    <span class="arrow">→</span>
                </button>
            </form>
            
            <form action="/setmode" method="POST" style="margin: 0;">
                <input type="hidden" name="mode" value="vumeter">
                <button type="submit" class="mode-button">
                    <span class="mode-icon">🎵</span>
                    <div class="mode-text">
                        <div>VU Meter</div>
                        <div class="mode-description">Audio visualization display</div>
                    </div>
                    <span class="arrow">→</span>
                </button>
            </form>
        </div>
    </div>
</body>
</html>
)rawString";