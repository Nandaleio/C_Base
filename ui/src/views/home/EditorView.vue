<script setup lang="ts">
    import { env } from '@/utils/env';
    import { ref } from 'vue';

    const sqlQuery = ref('');
    const list = ref([]);

    async function query() {
        const token = localStorage.getItem(env.localStorageTokenKey) 
        const res = await fetch(`${env.apiURL}/api/query`, {
            method: "POST",
            headers: {
                Authorization: `Bearer ${token}` 
            },
            body: JSON.stringify({query: sqlQuery.value})
        })
        console.log('res',res)
        if(res.ok) {
            list.value = await res.json();
            
            console.log('value',list.value)
        }

    }
</script>

<template>
    <div class="main-container">
        <textarea v-model="sqlQuery" name="sqlQuery"></textarea>
        <button @click="query">run</button>
        <div class="content">
            <table v-if="list && list.length">
                <thead>
                    <tr>
                    <th>Name</th>
                    <th>Home Runs</th>
                    <th>AVG</th>
                    </tr>
                </thead>
                <tbody>
                    <tr>
                    <td>Mike Trout</td>
                    <td>45</td>
                    <td>.291</td>
                    </tr>
                    <tr>
                    <td>Christian Yelich</td>
                    <td>44</td>
                    <td>.329</td>
                    </tr>
                    <tr>
                    <td>Mookie Betts</td>
                    <td>29</td>
                    <td>.295</td>
                    </tr>
                    <tr>
                    <td>Cody Bellinger</td>
                    <td>47</td>
                    <td>.305</td>
                    </tr>
                </tbody>
            </table>

        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    flex-direction: column;
    padding: 1rem;
    gap: 1rem;
    
    .content {

        table {
            width: 100%;
        }
    }

}

</style>