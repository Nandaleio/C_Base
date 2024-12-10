<script lang="ts" setup>
import {
    Table,
    TableBody,
    TableCell,
    TableHead,
    TableHeader,
    TableRow,
} from '@/components/ui/table'
import type { Cols } from '@/utils/types';

defineProps<{
    cols: Cols[],
    data: { [key: string]: any }
}>();
</script>

<template>
    <Table>
        <TableHeader>
            <TableRow>
                <TableHead v-for="col of cols">
                    {{ col.name }}
                </TableHead>
            </TableRow>
        </TableHeader>
        <TableBody>
            <TableRow v-for="d of data">
                <TableCell v-for="col of cols">
                    {{ d[col.name] }}
                </TableCell>
            </TableRow>
        </TableBody>
    </Table>
</template>