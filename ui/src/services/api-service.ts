import { useToast } from "@/components/ui/toast";

export async function cbFetch<T = any>(url: string, method:string = "GET", body?: any) {
    
    const token = localStorage.getItem(import.meta.env.VITE_LOCALSTORAGE_TOKEN_KEY) 
    const res = await fetch(`${import.meta.env.VITE_API_URL}${url}`, {
        method: method,
        headers: {
            Authorization: `Bearer ${token}` 
        },
        body: JSON.stringify(body)
    })
    if(!res.ok) {
        const { toast } = useToast()
        toast({
            title: 'Uh oh! Something went wrong.',
            description: res.statusText,
            variant: 'destructive',
          });
        throw new Error(res.statusText);
    }
    const ret = await res.json() as T;
    if((ret as any).error) {
        console.error((ret as any).error);
        const { toast } = useToast()
        toast({
            title: 'Uh oh! Something went wrong.',
            description: (ret as any).error,
            variant: 'destructive',
        });
    }
    return ret;
}