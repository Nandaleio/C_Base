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
    import { ScrollArea } from "@/components/ui/scroll-area"
    import { cbFetch } from '@/services/api-service';
    import { onMounted, ref } from 'vue';

    const loading = ref(false);
    const data = ref<{created: Date, level: string, description: string}[]>([]);
    
    async function query() {
        loading.value = true;
        const res = await cbFetch('/api/admin/logs')
        data.value = res.data;
        loading.value = false;
    }

    onMounted(async () => {
        await query();
    })

</script>

<template>
        <div class="logs-container p-4">
            <div class="actions">
                <Button variant="outline" @click="query" :aria-busy="loading">Refresh</Button>
            </div>
            <Table>
                <TableHeader>
                <TableRow>
                    <TableHead>Created</TableHead>
                    <TableHead>Level</TableHead>
                    <TableHead>Description</TableHead>
                </TableRow>
                </TableHeader>
                <TableBody>
                    <TableRow v-for="d of data">
                        <TableCell>{{d.created}}</TableCell>
                        <TableCell>{{d.level}}</TableCell>
                        <TableCell>{{d.description}}</TableCell>
                    </TableRow>
                </TableBody>
            </Table>
        </div>
</template>