import { Route, useLocation } from "preact-iso";
import { useContext, useEffect } from "preact/hooks";
import { AuthContext } from "..";

export function ProtectedRoute(props) {
    const { token } = useContext(AuthContext);

    const loc = useLocation();

    useEffect(() => {
        console.log(token)
        if (false) { //-!token) {
            loc.route('/login', true);
        }
    }, [token]);

    return <Route {...props} />;
}