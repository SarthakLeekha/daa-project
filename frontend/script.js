async function calculateDistance() {
    const string1 = document.getElementById('string1').value;
    const string2 = document.getElementById('string2').value;
    const resultDiv = document.getElementById('result');
    const errorDiv = document.getElementById('error');
    const errorMessage = document.getElementById('error-message');
    
    resultDiv.classList.remove('show');
    errorDiv.classList.remove('show');
    
    if (string1.trim() === '' || string2.trim() === '') {
        errorMessage.textContent = 'Please enter both strings';
        errorDiv.classList.add('show');
        return;
    }
    
    if (string1.length > 100 || string2.length > 100) {
        errorMessage.textContent = 'Strings must be 100 characters or less';
        errorDiv.classList.add('show');
        return;
    }
    
    try {
        const payload = JSON.stringify({
            s1: string1,
            s2: string2
        });
        
        
        const tryFetch = async (port) => {
            return await fetch(`http://127.0.0.1:${port}/calculate`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: payload
            });
        };

        let response = null;
        try {
            response = await tryFetch(5000);
            if (!response.ok) {
                // try fallback port
                response = await tryFetch(8080);
            }
        } catch (e) {
            // try fallback port
            response = await tryFetch(8080);
        }
        if (!response || !response.ok) {
            throw new Error('Network response was not ok');
        }
        
        const data = await response.json();
        document.getElementById('distance-value').textContent = data.distance;
        resultDiv.classList.add('show');
        
    } catch (error) {
        errorMessage.textContent = 'Error connecting to server. Make sure backend is running on port 5000';
        errorDiv.classList.add('show');
    }
}

document.getElementById('string1').addEventListener('keypress', function(event) {
    if (event.key === 'Enter') {
        calculateDistance();
    }
});

document.getElementById('string2').addEventListener('keypress', function(event) {
    if (event.key === 'Enter') {
        calculateDistance();
    }
});
