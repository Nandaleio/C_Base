<script setup lang="ts">
    import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
    } from '@/components/ui/table'
    import { Button } from '@/components/ui/button'
    import { Textarea } from '@/components/ui/textarea'

    import { cbFetch } from '@/services/api-service';
    import { ref } from 'vue';

    const sqlQuery = ref('');

    const loading = ref(false);
    const error = ref();
    const cols = ref([]);
    const data = ref([]);

    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/admin/query', "POST", {query: sqlQuery.value});
        cols.value = res.columns;
        data.value = res.data;
        error.value = res.error;
        loading.value = false;
    }
</script>

<template>
    <div class="main-container p-4">
        <Textarea v-model="sqlQuery" name="sqlQuery"></Textarea>
        <div class="actions">
            <Button @click="query">run</Button>
        </div>
        <div class="content" :aria-busy="loading">
            <Table v-if="cols && cols.length">
                <TableHeader>
                <TableRow>
                    <TableHead v-for="col of cols">
                    {{col}}
                    </TableHead>
                </TableRow>
                </TableHeader>
                <TableBody>
                <TableRow v-for="d of data">
                    <TableCell v-for="col of cols">
                    {{ d[col] }}
                    </TableCell>
                </TableRow>
                </TableBody>
            </Table>
            <pre v-else-if="error" class="error">{{ error }}</pre>
        </div>
    </div>
</template>

<style scoped>

.main-container{
    display: flex;
    flex-direction: column;
    gap: 1rem;
    
    .actions{
        display: flex;
    }

    .content {

        display: flex;
        align-items: flex-start;
        justify-content: center;
        height: 100%;

        .error{
            color: red;
        }
    }

}

</style>