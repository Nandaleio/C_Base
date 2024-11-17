import { useContext, useState } from "preact/hooks";
import { AuthContext } from "..";
import { useLocation } from "preact-iso";

export function Login() {

    const { token, setToken } = useContext(AuthContext);

    const [form, setForm] = useState<{username:string, password: string}>({username: '', password: ''})

    const loc = useLocation();

    const submitLoginForm = async (e: SubmitEvent) => {
        e.preventDefault();
        console.log(`${form.username}:${form.password}`)
        const res = await fetch('http://localhost:8080/api/login', {
            headers: { Authorization: 'Basic ' + btoa(form.username + ':' + form.password) }
        })
        if(res.ok) {
            const authToken = (await res.json()).token;
            setToken(authToken);
            document.cookie = `access_token=${token};path=/;max-age=3600`;
            loc.route('/tables');
        }
    }
    
    const usernameInput = e => {
        setForm({...form, username: e.currentTarget.value});
    }
    const passwordInput = e => {
        setForm({...form, password: e.currentTarget.value});
    }

    const style = {
        display: 'flex',
        "justify-content": 'center',
        "align-items": 'center',
        "flex-direction": "column",
        'width': '100%'
   };

    return (
    <div style={style}>
        <h3>Admin Login</h3>
        <form id="loginForm" onSubmit={submitLoginForm}>
                <div>
                    <label for="name">Username</label>
                    <input type="text" name="username" required value={form.username} onInput={usernameInput}/>
                </div>
                <div>
                    <label for="email">Password</label>
                    <input type="password" name="password" required value={form.password} onInput={passwordInput} autoComplete={"current-password"}/>
                </div>
                <input type="submit" value="Login" />
            </form>
        </div>
    )
}