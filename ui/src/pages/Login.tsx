import { useContext } from "preact/hooks";
import { AuthContext } from "..";

export function Login() {

    const { user, setUser } = useContext(AuthContext);

    const state = { 
        user: '',
        pass: ''
     };

    const submitLoginForm = async (e: SubmitEvent) => {
        e.preventDefault();
        console.log(e.target)
        const res = await fetch("/login", {
            method: "POST", 
            headers: { Authorization: 'Basic ' + btoa(state.user+':' + state.pass) }
        });
        if(res.ok) {
            const authToken = (await res.json()).token;
            setUser({token:authToken});
        }
    }

    const style = {
        display: 'flex',
        "justify-content": 'center',
        "align-items": 'center',
        "flex-direction": "column"
   };

    return (
    <div style={style}>
    <h3>Admin Login</h3>
    <form id="loginForm" onSubmit={submitLoginForm}>
            <div>
                <label for="name"> username </label>
                <input type="text" name="username" required value={state.user}/>
            </div>
            <div>
                <label for="email"> password </label>
                <input type="password" name="password" required value={state.pass}/>
            </div>
            <div>
                <input type="submit" value="Login" />
            </div>
        </form>
        </div>
    )
}