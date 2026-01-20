/*
  ESP32 HTML WebServer Page Code
 http:://www.electronicwings.com
*/

const char html_page[] PROGMEM = R"rawSrting(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Message Controller</title>
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
            padding: 30px;
            max-width: 500px;
            width: 100%;
        }
        
        .header {
            text-align: center;
            margin-bottom: 25px;
        }
        
        h1 {
            color: #667eea;
            font-size: 24px;
            margin-bottom: 10px;
        }
        
        .session-info {
            background: #f7fafc;
            border-left: 4px solid #667eea;
            padding: 12px 15px;
            border-radius: 6px;
            margin-bottom: 25px;
        }
        
        .session-info p {
            color: #4a5568;
            font-size: 14px;
            margin: 4px 0;
        }
        
        .session-info strong {
            color: #2d3748;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        label {
            display: block;
            color: #2d3748;
            font-weight: 600;
            margin-bottom: 8px;
            font-size: 14px;
        }
        
        input[type="text"],
        input[type="number"] {
            width: 100%;
            padding: 12px 15px;
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            font-size: 16px;
            transition: all 0.3s ease;
            font-family: inherit;
        }
        
        input[type="text"]:focus,
        input[type="number"]:focus {
            outline: none;
            border-color: #667eea;
            box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
        }
        
        .char-count {
            text-align: right;
            font-size: 12px;
            color: #718096;
            margin-top: 4px;
        }
        
        .number-inputs {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
        }
        
        button {
            width: 100%;
            padding: 14px;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 600;
            cursor: pointer;
            transition: transform 0.2s ease, box-shadow 0.2s ease;
            margin-top: 10px;
        }
        
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 10px 20px rgba(102, 126, 234, 0.3);
        }
        
        button:active {
            transform: translateY(0);
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 20px;
            }
            
            h1 {
                font-size: 20px;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>ESP32 Message Controller</h1>
        </div>
        
        <div class="session-info">
            <p><strong>Session Started:</strong></p>
            <p id="timestamp"></p>
        </div>
        
        <form action="http://ESP32server.local/submit" method="POST" accept-charset="ISO-8859-1">
            <div class="form-group">
                <label for="topText">Top Text Message</label>
                <input type="text" id="topText" name="topText" maxlength="11" required autocomplete="off" autocorrect="off" autocapitalize="off" spellcheck="false">
                <div class="char-count"><span id="topCount">0</span>/11</div>
            </div>
            
            <div class="form-group">
                <label for="upperText">Upper Text Message</label>
                <input type="text" id="upperText" name="upperText" maxlength="100" required autocomplete="off" autocorrect="off" autocapitalize="off" spellcheck="false">
                <div class="char-count"><span id="upperCount">0</span>/100</div>
            </div>
            
            <div class="form-group">
                <label for="lowerText">Lower Text Message</label>
                <input type="text" id="lowerText" name="lowerText" maxlength="11" required autocomplete="off" autocorrect="off" autocapitalize="off" spellcheck="false">
                <div class="char-count"><span id="lowerCount">0</span>/11</div>
            </div>
            
            <div class="number-inputs">
                <div class="form-group">
                    <label for="speed">Speed</label>
                    <input type="number" id="speed" name="speed" min="1" max="100" required>
                </div>
                
                <div class="form-group">
                    <label for="background">Background</label>
                    <input type="number" id="background" name="background" min="1" max="100" required>
                </div>
            </div>
            
            <button type="submit">Submit Messages</button>
        </form>
    </div>
    
    <script>
        // Set timestamp when page loads
        const now = new Date();
        const options = { 
            weekday: 'long', 
            year: 'numeric', 
            month: 'long', 
            day: 'numeric',
            hour: '2-digit',
            minute: '2-digit',
            second: '2-digit'
        };
        document.getElementById('timestamp').textContent = now.toLocaleString('en-US', options);
        
        // Character counters
        const topInput = document.getElementById('topText');
        const upperInput = document.getElementById('upperText');
        const lowerInput = document.getElementById('lowerText');
        const topCount = document.getElementById('topCount');
        const upperCount = document.getElementById('upperCount');
        const lowerCount = document.getElementById('lowerCount');
        
        topInput.addEventListener('input', function() {
            topCount.textContent = this.value.length;
        });
        
        upperInput.addEventListener('input', function() {
            upperCount.textContent = this.value.length;
        });
        
        lowerInput.addEventListener('input', function() {
            lowerCount.textContent = this.value.length;
        });
    </script>
</body>
</html>
)rawSrting";
    